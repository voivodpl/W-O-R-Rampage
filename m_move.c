/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// m_move.c -- monster movement

#include "g_local.h"

#define	STEPSIZE	18
void M_addslide(edict_t *self, vec3_t move);
/*
=============
M_CheckBottom

Returns false if any part of the bottom of the entity is off an edge that
is not a staircase.

=============
*/
int c_yes, c_no;

qboolean M_CheckBottom (edict_t *ent)
{
	vec3_t	mins, maxs, start, stop;
	trace_t	trace;
	int		x, y;
	float	mid, bottom;
	
	VectorAdd (ent->s.origin, ent->mins, mins);
	VectorAdd (ent->s.origin, ent->maxs, maxs);

// if all of the points under the corners are solid world, don't bother
// with the tougher checks
// the corners must be within 16 of the midpoint
	start[2] = mins[2] - 1;
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = x ? maxs[0] : mins[0];
			start[1] = y ? maxs[1] : mins[1];
			if (gi.pointcontents (start) != CONTENTS_SOLID)
				goto realcheck;
		}

	c_yes++;
	return true;		// we got out easy

realcheck:
	c_no++;
//
// check it for real...
//
	start[2] = mins[2];
	
// the midpoint must be within 16 of the bottom
	start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
	start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
	stop[2] = start[2] - 2 * STEPSIZE * 10; //we want monsters to jump out of high ground //woju
	trace = gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID);

	if (trace.fraction == 1.0)
		return false;
	mid = bottom = trace.endpos[2];
	
// the corners must be within 16 of the midpoint	
	for	(x=0 ; x<=1 ; x++)
		for	(y=0 ; y<=1 ; y++)
		{
			start[0] = stop[0] = x ? maxs[0] : mins[0];
			start[1] = stop[1] = y ? maxs[1] : mins[1];
			
			trace = gi.trace (start, vec3_origin, vec3_origin, stop, ent, MASK_MONSTERSOLID);
			
			if (trace.fraction != 1.0 && trace.endpos[2] > bottom)
				bottom = trace.endpos[2];
			if (trace.fraction == 1.0 || mid - trace.endpos[2] > STEPSIZE * 10)
				return false;
		}

	c_yes++;
	return true;
}


/*
=============
SV_movestep

Called by monster program code.
The move will be adjusted for slopes and stairs, but if the move isn't
possible, no move is done, false is returned, and
pr_global_struct->trace_normal is set to the normal of the blocking wall
=============
*/
//FIXME since we need to test end position contents here, can we avoid doing
//it again later in catagorize position?
qboolean SV_movestep (edict_t *ent, vec3_t move, qboolean relink)
{
	float		dz;
	vec3_t		oldorg, neworg, end;
	trace_t		trace;
	int			i;
	float		stepsize;
	vec3_t		test;
	int			contents;


	
// try the move	
	VectorCopy (ent->s.origin, oldorg);
	VectorAdd (ent->s.origin, move, neworg);



// flying monsters don't step up
	if ( ent->flags & (FL_SWIM | FL_FLY) )
	{
	// try one move with vertical motion, then one without
		for (i=0 ; i<2 ; i++)
		{
			VectorAdd (ent->s.origin, move, neworg);
			if (i == 0 && ent->enemy)
			{
				if (!ent->goalentity)
					ent->goalentity = ent->enemy;
				dz = ent->s.origin[2] - ent->goalentity->s.origin[2];
				if (ent->goalentity->client)
				{
					if (dz > 40)
						neworg[2] -= 8;
					if (!((ent->flags & FL_SWIM) && (ent->waterlevel < 2)))
						if (dz < 30)
							neworg[2] += 8;
				}
				else
				{
					if (dz > 8)
						neworg[2] -= 8;
					else if (dz > 0)
						neworg[2] -= dz;
					else if (dz < -8)
						neworg[2] += 8;
					else
						neworg[2] += dz;
				}
			}
			trace = gi.trace (ent->s.origin, ent->mins, ent->maxs, neworg, ent, MASK_MONSTERSOLID);
	
			// fly monsters don't enter water voluntarily
			if (ent->flags & FL_FLY)
			{
				if (!ent->waterlevel)
				{
					test[0] = trace.endpos[0];
					test[1] = trace.endpos[1];
					test[2] = trace.endpos[2] + ent->mins[2] + 1;
					contents = gi.pointcontents(test);
					if (contents & MASK_WATER)
						return false;
				}
			}

			// swim monsters don't exit water voluntarily
			if (ent->flags & FL_SWIM)
			{
				if (ent->waterlevel < 2)
				{
					test[0] = trace.endpos[0];
					test[1] = trace.endpos[1];
					test[2] = trace.endpos[2] + ent->mins[2] + 1;
					contents = gi.pointcontents(test);
					if (!(contents & MASK_WATER))
						return false;
				}
			}

			if (trace.fraction == 1)
			{
				check_move_dir(ent, trace.endpos);

				VectorCopy (trace.endpos, ent->s.origin);
				if (relink)
				{
					gi.linkentity (ent);
					G_TouchTriggers (ent);	
				}
				return true;
			}
			
			if (!ent->enemy)
				break;
		}
		
		return false;
	}

// push down from a step height above the wished position
	if (!(ent->monsterinfo.aiflags & AI_NOSTEP))
		stepsize = STEPSIZE;
	else
		stepsize = 1;

	neworg[2] += stepsize;
	VectorCopy (neworg, end);
	end[2] -= stepsize*2;

	trace = gi.trace (neworg, ent->mins, ent->maxs, end, ent, MASK_MONSTERSOLID);

	if (trace.allsolid)
		return false;

	if (trace.startsolid)
	{
		neworg[2] -= stepsize;
		trace = gi.trace (neworg, ent->mins, ent->maxs, end, ent, MASK_MONSTERSOLID);
		if (trace.allsolid || trace.startsolid)
			return false;
	}


	// don't go in to water
	if (ent->waterlevel == 2)
	{
		test[0] = trace.endpos[0];
		test[1] = trace.endpos[1];
		test[2] = trace.endpos[2] + ent->mins[2] - 1;	
		contents = gi.pointcontents(test);

		if (contents & MASK_WATER)
			return false;
	}

	if (trace.fraction == 1)
	{
	// if monster had the ground pulled out, go ahead and fall
		if ( ent->flags & FL_PARTIALGROUND )
		{
			VectorAdd (ent->s.origin, move, ent->s.origin);

			if (relink)
			{
				gi.linkentity (ent);
				G_TouchTriggers (ent);
			}
			ent->groundentity = NULL;
			return true;
		}
	
		return false;		// walked off an edge
	}

// check point traces down for dangling corners
	check_move_dir(ent, trace.endpos);

	
	VectorCopy (trace.endpos, ent->s.origin);

	if (!M_CheckBottom (ent))
	{
		if ( ent->flags & FL_PARTIALGROUND )
		{	// entity had floor mostly pulled out from underneath it
			// and is trying to correct
			if (relink)
			{
				gi.linkentity (ent);
				G_TouchTriggers (ent);
			}
			return true;
		}
		VectorCopy (oldorg, ent->s.origin);
		return false;
	}
	VectorCopy(oldorg, ent->old_origin2);
	//add some sliding
	M_addslide(ent, move);
	if ( ent->flags & FL_PARTIALGROUND )
	{
		ent->flags &= ~FL_PARTIALGROUND;
	}
	ent->groundentity = trace.ent;
	ent->groundentity_linkcount = trace.ent->linkcount;

// the move is ok
	if (relink)
	{


		gi.linkentity (ent);
		G_TouchTriggers (ent);
	}
	return true;
}


//============================================================================

/*
===============
M_ChangeYaw

===============
*/
void M_ChangeYaw (edict_t *ent)
{
	float	ideal;
	float	current;
	float	move;
	float	speed;
	vec3_t temp;
	current = anglemod(ent->s.angles[YAW]);
	if (ent->enemy && visible(ent, ent->enemy) && !(ent->svflags & SVF_NOANGLECHASE) &&
		!(ent->monsterinfo.monster_type == MONSTER_SOLDIER && check_frames(ent, Sol_death5_start, Sol_death5_end)))
	{
		vec3_t prediction, vec;
		//VectorCopy(ent->enemy->s.old_origin, prediction);
		//VectorMA(prediction, FRAMETIME * 3 * (0.9 + (random() * 0.2)), ent->enemy->velocity, prediction);
		predict_shot(ent, ent->s.origin, -1, prediction, TYPE_HITSCAN); //predict without correction
		VectorSubtract(prediction, ent->s.origin, vec);
		
		VectorCopy(vec, temp);
		ideal = vectoyaw(temp);
		//VectorNormalize(vec);
		
		//gi.bprintf(PRINT_HIGH, "see enemy? = %i\n", enemy_vis);
		

		//gi.bprintf(PRINT_HIGH, "DEBUG: TRIED %i times vectoyaw", i);
		if (ideal < -1000) // this fixes the -1.#ind bug from vectoyaw - > atan2, no idea why it sometimes gives #ind
		{
			VectorCopy(vec, temp);
			//VectorSet(temp, -100, -100, -100);
			//vec[0] = 1;
			//VectorCopy(vec, temp);
			ideal = vectoyaw(temp);
			gi.bprintf(PRINT_HIGH, "DEBUG: vectoyaw gave IND, trying second time\n");
			//gi.bprintf(PRINT_HIGH, "DEBUG: IDEAL = %f prediction = %f %f %f, origin = %f %f %f, temp = %f %f %f\n", ideal, prediction[0], prediction[1], prediction[2], ent->s.origin[0], ent->s.origin[1], ent->s.origin[2], vec[0], vec[1], vec[2]);
		}
		//
		if (ideal < -1000)
			gi.bprintf(PRINT_HIGH, "DEBUG: vectoyaw gave IND\n");
		/*gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_ROCKET_EXPLOSION);
		gi.WritePosition(prediction);
		gi.multicast(prediction, MULTICAST_PHS);*/

	}
	else
	{
		ideal = ent->ideal_yaw;
	}
		

	if (current == ideal)
		return;

	move = ideal - current;

	if(ent->enemy)
		speed = ent->yaw_speed;
	else if(ent->monsterinfo.aiflags & AI_IMPATIENT)
		speed = ent->yaw_speed * 0.5;
	else 
		speed = ent->yaw_speed * 0.25;

	int temp_health = ent->health + 100;
		  
		speed *= 1.75 + (((temp_health / (ent->max_health + 100) ) * 0.25));
		if (ent->health <= 0)
			speed *= 0.25;
		//if (check_frames(ent, Sol_death5_start, Sol_death5_end))
		//	speed *= 0.25;

	if (ideal > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > speed)
			move = speed;
	}
	else
	{
		if (move < -speed)
			move = -speed;
	}
	
	ent->s.angles[YAW] = anglemod (current + move);
}


/*
======================
SV_StepDirection

Turns to the movement direction, and walks the current distance if
facing it.

======================
*/
qboolean seehas_enemy(edict_t *ent)
{
	if (!ent->enemy)
		return true;
	if(visible(ent, ent->enemy))
		return false;

	return true;
}
qboolean SV_StepDirection (edict_t *ent, float yaw, float dist)
{
	vec3_t		move, oldorigin;
	float		delta;
	
	ent->ideal_yaw = yaw;
	M_ChangeYaw (ent);
	
	yaw = yaw*M_PI*2 / 360;
	move[0] = cos(yaw)*dist;
	move[1] = sin(yaw)*dist;
	move[2] = 0;

	VectorCopy (ent->s.origin, oldorigin);
	if (SV_movestep (ent, move, false))
	{
		delta = ent->s.angles[YAW] - ent->ideal_yaw;
		if ((delta > 45 && delta < 315) && seehas_enemy(ent))
		{		// not turned far enough, so don't take the step
			//if (!ent->enemy && ent->count > 100000)
			//	gi.bprintf(PRINT_HIGH, "SV_StepDirection: not turned far enough, so don't take the step\n");

			//ent->count++;

			VectorCopy(oldorigin, ent->s.origin);
		}
		//else
			//ent->count = 0;
		gi.linkentity (ent);
		G_TouchTriggers (ent);
		return true;
	}
	else if (ent->svflags & SVF_MONSTER)
	{
		VectorCopy(ent->s.origin, ent->old_origin2);
	}
	gi.linkentity (ent);
	G_TouchTriggers (ent);
	return false;
}

/*
======================
SV_FixCheckBottom

======================
*/
void SV_FixCheckBottom (edict_t *ent)
{
	ent->flags |= FL_PARTIALGROUND;
}



/*
================
SV_NewChaseDir

================
*/
#define	DI_NODIR	-1
void SV_NewChaseDir (edict_t *actor, edict_t *enemy, float dist)
{
	float	deltax,deltay;
	float	d[3];
	float	tdir, olddir, turnaround;

	//FIXME: how did we get here with no enemy
	if (!enemy)
		return;

	olddir = anglemod( (int)(actor->ideal_yaw/45)*45 );
	turnaround = anglemod(olddir - 180);

	deltax = enemy->s.origin[0] - actor->s.origin[0];
	deltay = enemy->s.origin[1] - actor->s.origin[1];
	if (deltax>10)
		d[1]= 0;
	else if (deltax<-10)
		d[1]= 180;
	else
		d[1]= DI_NODIR;
	if (deltay<-10)
		d[2]= 270;
	else if (deltay>10)
		d[2]= 90;
	else
		d[2]= DI_NODIR;

// try direct route
	if (d[1] != DI_NODIR && d[2] != DI_NODIR)
	{
		if (d[1] == 0)
			tdir = d[2] == 90 ? 45 : 315;
		else
			tdir = d[2] == 90 ? 135 : 215;
			
		if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
			return;
	}

// try other directions
	if ( ((rand()&3) & 1) ||  fabsf(deltay) > fabsf(deltax))
	{
		tdir=d[1];
		d[1]=d[2];
		d[2]=tdir;
	}

	if (d[1]!=DI_NODIR && d[1]!=turnaround 
	&& SV_StepDirection(actor, d[1], dist))
			return;

	if (d[2]!=DI_NODIR && d[2]!=turnaround
	&& SV_StepDirection(actor, d[2], dist))
			return;

/* there is no direct path to the player, so pick another direction */

	if (olddir!=DI_NODIR && SV_StepDirection(actor, olddir, dist))
			return;

	if (rand()&1) 	/*randomly determine direction of search*/
	{
		for (tdir=0 ; tdir<=315 ; tdir += 45)
			if (tdir!=turnaround && SV_StepDirection(actor, tdir, dist) )
					return;
	}
	else
	{
		for (tdir=315 ; tdir >=0 ; tdir -= 45)
			if (tdir!=turnaround && SV_StepDirection(actor, tdir, dist) )
					return;
	}

	if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist) )
			return;

	actor->ideal_yaw = olddir;		// can't move

// if a bridge was pulled out from underneath a monster, it may not have
// a valid standing position at all

	if (!M_CheckBottom (actor))
		SV_FixCheckBottom (actor);
}

/*
======================
SV_CloseEnough

======================
*/
qboolean SV_CloseEnough (edict_t *ent, edict_t *goal, float dist)
{
	int		i;
	
	for (i=0 ; i<3 ; i++)
	{
		if (goal->absmin[i] > ent->absmax[i] + dist)
			return false;
		if (goal->absmax[i] < ent->absmin[i] - dist)
			return false;
	}
	return true;
}


/*
======================
M_MoveToGoal
======================
*/
void M_MoveToGoal (edict_t *ent, float dist)
{
	edict_t		*goal;
	
	goal = ent->goalentity;

	if (!ent->groundentity && !(ent->flags & (FL_FLY | FL_SWIM)))
	{
		//ent->monsterinfo.velocity[2] = 12345;
		ent->monsterinfo.aiflags |= AI_NOTONGROUND1;
		ent->monsterinfo.aiflags &= ~AI_NOTONGROUND2;
		return;
	}
	else
	{
		if (ent->monsterinfo.aiflags & AI_NOTONGROUND1)
		{
			ent->monsterinfo.aiflags |= AI_NOTONGROUND2;
			ent->monsterinfo.aiflags &= ~AI_NOTONGROUND1;
		}
		else
			ent->monsterinfo.aiflags &= ~AI_NOTONGROUND2;
			
	}

// if the next step hits the enemy, return immediately
	if (ent->enemy &&  SV_CloseEnough(ent, ent->enemy, dist*0.5))
	{
		M_ChangeYaw(ent);
		if (!(ent->monsterinfo.aiflags & AI_COMBAT_POINT))
		{
			//gi.bprintf(PRINT_HIGH, "DEBUG: ENEMY IS CLOSE ENOUGH, DON'T DO ANYTHING! NO COMBAT POINT!\n");

			VectorCopy(ent->s.origin, ent->old_origin2);
			return;
		}
		else
		{
			ent->monsterinfo.aiflags &= ~AI_COMBAT_POINT;

			//gi.bprintf(PRINT_HIGH, "DEBUG: ENEMY IS CLOSE ENOUGH, DON'T DO ANYTHING! REMOVE COMBAT POINT!\n");

		}
			
	}
		

// bump around...
	if ( (rand()&3)==1 || !SV_StepDirection (ent, ent->ideal_yaw, dist))
	{
		if (ent->inuse)
			SV_NewChaseDir (ent, goal, dist);
	}
}


/*
===============
M_walkmove
===============
*/
qboolean M_walkmove (edict_t *ent, float yaw, float dist)
{
	vec3_t	move;
	
	if (!ent->groundentity && !(ent->flags & (FL_FLY|FL_SWIM)))
		return false;

	yaw = yaw*M_PI*2 / 360;
	
	move[0] = cos(yaw)*dist;
	move[1] = sin(yaw)*dist;
	move[2] = 0;

	return SV_movestep(ent, move, true);
}

void M_addslide(edict_t *self, vec3_t move)
{
	return; //FIXME// Under development.

	if (!self->enemy || !self->groundentity)
		return;

	vec3_t oldmove_org, oldmove;
	float difference;

	VectorSubtract(self->old_origin2, self->s.origin, oldmove);
	VectorCopy(oldmove, oldmove_org);
	//oldmove[2] = 0;
	//move[2] = 0;
	VectorNormalize(oldmove);
	VectorNormalize(move);

	difference = DotProduct(oldmove, move);
	
	VectorScale(oldmove, 10, oldmove);
	VectorAdd(oldmove, self->velocity, self->velocity);
	gi.bprintf(PRINT_HIGH, "DEBUG: oldmove = %s, move = %s, difference = %f\n", vtos(oldmove), vtos(move), difference);
}
