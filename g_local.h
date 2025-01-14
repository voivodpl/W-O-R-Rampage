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
// g_local.h -- local definitions for game module

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN	//non-MFC
#include <windows.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
_CrtMemState startup1;	// memory diagnostics
#else
#define OutputDebugString	//not doing Windows
#endif

#include "q_shared.h"

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define	GAME_INCLUDE
#include "game.h"
//#include "mtwist.h"
// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"Rampage v1.4o"
#define GAMEDATE	__DATE__

// protocol bytes that can be directly added to messages
#define	svc_muzzleflash		1
#define	svc_muzzleflash2	2
#define	svc_temp_entity		3
#define	svc_layout			4
#define	svc_inventory		5
#define	svc_stufftext		11

//==================================================================

// view pitching times
#define DAMAGE_TIME		0.5f
#define	FALL_TIME		0.3f


// edict->spawnflags
// these are set with checkboxes on each entity in the map editor
#define	SPAWNFLAG_NOT_EASY			0x00000100
#define	SPAWNFLAG_NOT_MEDIUM		0x00000200
#define	SPAWNFLAG_NOT_HARD			0x00000400
#define	SPAWNFLAG_NOT_DEATHMATCH	0x00000800
#define	SPAWNFLAG_NOT_COOP			0x00001000

// edict->flags
#define	FL_FLY					0x00000001
#define	FL_SWIM					0x00000002	// implied immunity to drowining
#define FL_IMMUNE_LASER			0x00000004
#define	FL_INWATER				0x00000008
#define	FL_GODMODE				0x00000010
#define	FL_NOTARGET				0x00000020
#define FL_IMMUNE_SLIME			0x00000040
#define FL_IMMUNE_LAVA			0x00000080
#define	FL_PARTIALGROUND		0x00000100	// not all corners are valid
#define	FL_WATERJUMP			0x00000200	// player jumping out of water
#define	FL_TEAMSLAVE			0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK			0x00000800
#define FL_POWER_ARMOR			0x00001000	// power armor (if any) is active
#define FL_KICK					0x00002000 //player is kicking
#define FL_QUAD					0x00004000  //using quad etc.
#define FL_INV					0x00008000 
#define FL_ENV					0x00010000 //
#define FL_REB					0x00020000 //
#define FL_UNDERWATER			0x00040000 //player used rebreather or enviro underwater, so he still drowns
#define FL_HOLSTER_R			0x00080000 // activating rebreather
#define FL_ACTIVATING_R			0x00100000 // activating rebreather
#define FL_HOLSTER_E			0x00200000 // activating enviro
#define FL_ACTIVATING_E			0x00400000 // activating enviro
#define FL_DEACTIVATING_R		0x00800000 // deactivating rebreather
#define FL_DEACTIVATING_E		0x01000000 // deaactivating enviro
#define FL_GENERATOR			0x02000000 //power generator
#define FL_NUKE					0x04000000 //this is a nuke!
#define FL_HEADSHOT				0x08000000 	//last damage was headshot
#define FL_RESPAWN				0x80000000	// used for item respawning


#define	FRAMETIME		0.1f

// memory tags to allow dynamic memory to be cleaned up
#define	TAG_GAME	765		// clear when unloading the dll
#define	TAG_LEVEL	766		// clear when loading a new level


#define MELEE_DISTANCE	80

#define BODY_QUEUE_SIZE		8

typedef enum
{
	DAMAGE_NO,
	DAMAGE_YES,			// will take damage if hit
	DAMAGE_AIM, 	// auto targeting recognizes this
	DAMAGE_PUSH // only kick entities, doesn't do damage
} damage_t;



typedef enum 
{
	WEAPON_READY, 
	WEAPON_ACTIVATING,
	WEAPON_DROPPING,
	WEAPON_FIRING,
	WEAPON_SWITCH_MODE,
	WEAPON_SWITCH_DUAL,
	WEAPON_RELOADING,
	WEAPON_CHARGINGUP
} weaponstate_t;

typedef enum
{
	AMMO_BULLETS,
	AMMO_SHELLS,
	AMMO_ROCKETS,
	AMMO_GRENADES,
	AMMO_CLUSTER_GRENADES,
	AMMO_CELLS,
	AMMO_SLUGS,
	AMMO_CHARGE
} ammo_t;


//deadflag
#define DEAD_NO					0
#define DEAD_DYING				1
#define DEAD_DEAD				2
#define DEAD_RESPAWNABLE		3

//range
#define RANGE_MELEE				0
#define RANGE_NEAR				1
#define RANGE_MID				2
#define RANGE_FAR				3
#define RANGE_VERYFAR			4
//gib types
#define GIB_ORGANIC				0
#define GIB_METALLIC			1

//monster ai flags
#define AI_STAND_GROUND			0x00000001
#define AI_TEMP_STAND_GROUND	0x00000002
#define AI_SOUND_TARGET			0x00000004
#define AI_LOST_SIGHT			0x00000008
#define AI_PURSUIT_LAST_SEEN	0x00000010
#define AI_PURSUE_NEXT			0x00000020
#define AI_PURSUE_TEMP			0x00000040
#define AI_HOLD_FRAME			0x00000080
#define AI_GOOD_GUY				0x00000100
#define AI_BRUTAL				0x00000200
#define AI_NOSTEP				0x00000400
#define AI_DUCKED				0x00000800
#define AI_COMBAT_POINT			0x00001000
#define AI_MEDIC				0x00002000
#define AI_RESURRECTING			0x00004000
#define AI_SHOOTGRENADE			0x00008000 //NEXT ATTACK SHOOTS GRENADE
#define AI_NOREFIRE				0x00010000 
#define AI_SHOOTRAILGUNFLAK		0x00020000 
#define AI_SHOOTSROCKET			0x00040000 	//super rocket...
#define AI_CHARGEDSHOT			0x00080000 
#define AI_RELOADSECONDARY		0x00100000 	
#define AI_JUMPDODGE			0x00200000 	
#define AI_JUMPDODGEPROJ		0x00400000 	
#define AI_AIMED_AT				0x00800000 //if the monster is aimed at at current frame
#define AI_IMPATIENT			0x01000000 //monster is rotating faster on idle
#define AI_COVER_FIRE			0x02000000 
#define AI_PLAYER_STRAFING      0x04000000 //player is strafing sideways only, this is to prevent fooling the prediction function
#define AI_REVERSE_ANIM			0x08000000
#define AI_NOTONGROUND1			0x10000000
#define AI_NOTONGROUND2			0x20000000
#define AI_JUMPATTACK			0x40000000
//monster attack state
#define AS_STRAIGHT				1
#define AS_SLIDING				2
#define	AS_MELEE				3
#define	AS_MISSILE				4

// armor types
#define ARMOR_NONE				0
#define ARMOR_JACKET			1
#define ARMOR_COMBAT			2
#define ARMOR_BODY				3
#define ARMOR_SHARD				4

// power armor types
#define POWER_ARMOR_NONE		0
#define POWER_ARMOR_SCREEN		1
#define POWER_ARMOR_SHIELD		2

// handedness values
#define RIGHT_HANDED			0
#define LEFT_HANDED				1
#define CENTER_HANDED			2


// game.serverflags values
#define SFL_CROSS_TRIGGER_1		0x00000001
#define SFL_CROSS_TRIGGER_2		0x00000002
#define SFL_CROSS_TRIGGER_3		0x00000004
#define SFL_CROSS_TRIGGER_4		0x00000008
#define SFL_CROSS_TRIGGER_5		0x00000010
#define SFL_CROSS_TRIGGER_6		0x00000020
#define SFL_CROSS_TRIGGER_7		0x00000040
#define SFL_CROSS_TRIGGER_8		0x00000080
#define SFL_CROSS_TRIGGER_MASK	0x000000ff


// noise types for PlayerNoise
#define PNOISE_SELF				0
#define PNOISE_WEAPON			1
#define PNOISE_IMPACT			2


// edict->movetype values
typedef enum
{
MOVETYPE_NONE,			// never moves
MOVETYPE_NOCLIP,		// origin and angles change with no interaction
MOVETYPE_PUSH,			// no clip to world, push on box contact
MOVETYPE_STOP,			// no clip to world, stops on box contact
MOVETYPE_WALK,			// gravity
MOVETYPE_STEP,			// gravity, special edge handling
MOVETYPE_FLY,
MOVETYPE_FLY_B,
MOVETYPE_TOSS,			// gravity
MOVETYPE_FLYMISSILE,	// extra size to monsters
MOVETYPE_BOUNCE
} movetype_t;



typedef struct
{
	int		base_count;
	int		max_count;
	float	normal_protection;
	float	energy_protection;
	int		armor;
} gitem_armor_t;


// gitem_t->flags
#define	IT_WEAPON		1		// use makes active weapon
#define	IT_AMMO			2
#define IT_ARMOR		4
#define IT_STAY_COOP	8
#define IT_KEY			16
#define IT_POWERUP		32

// gitem_t->weapmodel for weapons indicates model index
#define WEAP_BLASTER			1 
#define WEAP_SHOTGUN			2 
#define WEAP_SUPERSHOTGUN		3 
#define WEAP_MACHINEGUN			4 
#define WEAP_CHAINGUN			5 
#define WEAP_GRENADES			6 
#define WEAP_GRENADELAUNCHER	7 
#define WEAP_ROCKETLAUNCHER		8 
#define WEAP_HYPERBLASTER		9 
#define WEAP_RAILGUN			10
#define WEAP_BFG				11

typedef struct gitem_s
{
	char		*classname;	// spawning name
	qboolean	(*pickup)(struct edict_s *ent, struct edict_s *other);
	void		(*use)(struct edict_s *ent, struct gitem_s *item);
	void		(*drop)(struct edict_s *ent, struct gitem_s *item);
	void		(*weaponthink)(struct edict_s *ent);
	char		*pickup_sound;
	char		*world_model;
	int			world_model_flags;
	char		*view_model;

	char		*second_view_model; //alternative view model
	int			mode; //weapon mode
	int			scounter; //counter used with weapon modes
	int			dual; //if weapon can be dual wielded

	// client side info
	char		*icon;
	char		*pickup_name;	// for printing on pickup
	int			count_width;		// number of digits to display by icon

	int			quantity;		// for ammo how much, for weapons how much is used per shot
	int			mag_prim;
	int			mag_sec;
	char		*ammo;			// for weapons
	int			flags;			// IT_* flags

	int			weapmodel;		// weapon model index (for weapons)

	void		*info;
	int			tag;

	char		*precaches;		// string of all models, sounds, and images this item will use
} gitem_t;

typedef struct
{
	int mode; //to save current mode of a weapon
	int mode2; //to save current mode of a second weapon
	int scounter; //used to count something when needed
	int dual; //if player has second dual wielded weapon and if the dual wield is active
	int last_shot;
	int mag_prim;
	int mag_sec;
	int	loopstarted;
	int mag_queue1;
	int mag_queue2;
} wext_t;

typedef struct 
{
	wext_t blaster;
	wext_t shotgun;
	wext_t supershotgun;
	wext_t machinegun;
	wext_t chaingun;
	wext_t grenades;
	wext_t grenadelauncher;
	wext_t rocketlauncher;
	wext_t hyperblaster;
	wext_t railgun;
	wext_t bfg10k;
} wexs_t;


//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
typedef struct
{
	char		helpmessage1[512];
	char		helpmessage2[512];
	int			helpchanged;	// flash F1 icon if non 0, play sound
								// and increment only if 1, 2, or 3

	gclient_t	*clients;		// [maxclients]

	// can't store spawnpoint in level, because
	// it would get overwritten by the savegame restore
	char		spawnpoint[512];	// needed for coop respawns

	// store latched cvars here that we want to get at often
	int			maxclients;
	int			maxentities;

	// cross level triggers
	int			serverflags;

	// items
	int			num_items;

	qboolean	autosaved;
} game_locals_t;


//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//
typedef struct
{
	int			framenum;
	float		time;

	char		level_name[MAX_QPATH];	// the descriptive name (Outer Base, etc)
	char		mapname[MAX_QPATH];		// the server name (base1, etc)
	char		nextmap[MAX_QPATH];		// go here when fraglimit is hit

	// intermission state
	float		intermissiontime;		// time the intermission was started
	char		*changemap;
	int			exitintermission;
	vec3_t		intermission_origin;
	vec3_t		intermission_angle;

	edict_t		*sight_client;	// changed once each frame for coop games

	edict_t		*sight_entity;
	int			sight_entity_framenum;
	edict_t		*sound_entity;
	int			sound_entity_framenum;
	edict_t		*sound2_entity;
	int			sound2_entity_framenum;

	int			pic_health;

	int			total_secrets;
	int			found_secrets;

	int			total_goals;
	int			found_goals;

	int			total_monsters;
	int			killed_monsters;

	edict_t		*current_entity;	// entity running from G_RunFrame
	int			body_que;			// dead bodies

	int			power_cubes;		// ugly necessity for coop
	int row_dmg_received;
	int row_dmg_dealt;
	int row_dmg_saved;
	int row_kills;
	int row_item_pickup;
	int row_item_usage;
	int row_health_bonus;
	int row_objectives;
	int row_secrets;
	int row_total;
	int show;
	edict_t *sun; //sun entity
} level_locals_t;


// spawn_temp_t is only used to hold entity field values that
// can be set from the editor, but aren't actualy present
// in edict_t during gameplay
typedef struct
{
	// world vars
	char		*sky;
	float		skyrotate;
	vec3_t		skyaxis;
	char		*nextmap;

	int			lip;
	int			distance;
	int			height;
	char		*noise;
	float		pausetime;
	char		*item;
	char		*gravity;

	float		minyaw;
	float		maxyaw;
	float		minpitch;
	float		maxpitch;
} spawn_temp_t;


typedef struct
{
	// fixed data
	vec3_t		start_origin;
	vec3_t		start_angles;
	vec3_t		end_origin;
	vec3_t		end_angles;

	int			sound_start;
	int			sound_middle;
	int			sound_end;

	float		accel;
	float		speed;
	float		decel;
	float		distance;

	float		wait;

	// state data
	int			state;
	vec3_t		dir;
	float		current_speed;
	float		move_speed;
	float		next_speed;
	float		remaining_distance;
	float		decel_distance;
	void		(*endfunc)(edict_t *);
} moveinfo_t;


typedef struct
{
	void	(*aifunc)(edict_t *self, float dist);
	float	dist;
	void	(*thinkfunc)(edict_t *self);
} mframe_t;

typedef struct
{
	int			firstframe;
	int			lastframe;
	mframe_t	*frame;
	void		(*endfunc)(edict_t *self);
} mmove_t;

typedef struct
{
	mmove_t		*currentmove;
	int			aiflags;
	int			nextframe;
	float		scale;

	void		(*stand)(edict_t *self);
	void		(*idle)(edict_t *self);
	void		(*search)(edict_t *self);
	void		(*walk)(edict_t *self);
	void		(*run)(edict_t *self);
	void		(*dodge)(edict_t *self, edict_t *other, float eta);
	void		(*attack)(edict_t *self);
	void		(*melee)(edict_t *self);
	void		(*sight)(edict_t *self, edict_t *other);
	qboolean	(*checkattack)(edict_t *self);

	float		pausetime;
	float		attack_finished;

	vec3_t		saved_goal;
	float		search_time;
	float		trail_time;
	vec3_t		last_sighting;
	vec3_t		last_sighting2;
	int			attack_state;
	int			lefty;
	float		idle_time;
	int			linkcount;

	int			power_armor_type;
	int			power_armor_power;
	int			emp_effect_left;
	float       aggression;
	int			charge;
	int			clip;
	int			move_dir;
	float		movedir_start;
	edict_t		*jump_ent;
	float		weapon_offset;
	float		last_dodge;
	int			monster_type;
	float		last_retreat;
	edict_t		*retreat_point;
	float		predict_skew_mult;
	float		predict_skew;
	float		last_enemy_velocity;
	vec3_t       velocity;
	vec3_t       oldvelocity;
	float		last_enemy_strafe;
	vec3_t	enemy_pos1;
	vec3_t	enemy_pos2;
	vec3_t	enemy_pos3;
	vec3_t	enemy_pos4;
	vec3_t	enemy_pos5;
	float	last_seen;
	int   temp_inaccuracy;
	vec3_t spread; //
	vec3_t aiming_at;
	float aimdur; //how long has the monster aimed (multiplier)
	int burstnum; //for monsters counting bursts shots
	float next_dir_change;

} monsterinfo_t;



extern	game_locals_t	game;
extern	level_locals_t	level;
extern	game_import_t	gi;
extern	game_export_t	globals;
extern	spawn_temp_t	st;

extern	int	sm_meat_index;
extern	int	snd_fry;

extern	int	jacket_armor_index;
extern	int	combat_armor_index;
extern	int	body_armor_index;


// means of death
#define MOD_UNKNOWN			0
#define MOD_BLASTER			1
#define MOD_SHOTGUN			2
#define MOD_SSHOTGUN		3
#define MOD_MACHINEGUN		4
#define MOD_CHAINGUN		5
#define MOD_GRENADE			6
#define MOD_G_SPLASH		7
#define MOD_ROCKET			8
#define MOD_R_SPLASH		9
#define MOD_HYPERBLASTER	10
#define MOD_RAILGUN			11
#define MOD_BFG_LASER		12
#define MOD_BFG_BLAST		13
#define MOD_BFG_EFFECT		14
#define MOD_HANDGRENADE		15
#define MOD_HG_SPLASH		16
#define MOD_WATER			17
#define MOD_SLIME			18
#define MOD_LAVA			19
#define MOD_CRUSH			20
#define MOD_TELEFRAG		21
#define MOD_FALLING			22
#define MOD_SUICIDE			23
#define MOD_HELD_GRENADE	24
#define MOD_EXPLOSIVE		25
#define MOD_BARREL			26
#define MOD_BOMB			27
#define MOD_EXIT			28
#define MOD_SPLASH			29
#define MOD_TARGET_LASER	30
#define MOD_TRIGGER_HURT	31
#define MOD_HIT				32
#define MOD_TARGET_BLASTER	33
#define MOD_HELD_GRAVITY_GRENADE 34
#define MOD_GRAVITY_GRENADE 35
#define MOD_RAILGUN_FRAG 36
#define MOD_MACHINEGUN_EMP 37
#define MOD_GRAPPLE 38
#define MOD_FRIENDLY_FIRE	0x8000000

extern	int	meansOfDeath;

#define q_offsetof(t, m)    ((size_t)&((t *)0)->m)

extern	edict_t* g_edicts;

#define FOFS(x)  q_offsetof(edict_t, x)
#define STOFS(x) q_offsetof(spawn_temp_t, x)
#define LLOFS(x) q_offsetof(level_locals_t, x)
#define CLOFS(x) q_offsetof(gclient_t, x)

#define	WSLOFS(x) (int)&(((wexs_t *)0)->x)
#define	WTLOFS(x) (int)&(((wext_t *)0)->x)

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

extern	cvar_t	*maxentities;
extern	cvar_t	*deathmatch;
extern	cvar_t	*coop;
extern	cvar_t	*dmflags;
extern	cvar_t	*skill;
extern	cvar_t	*fraglimit;
extern	cvar_t	*timelimit;
extern	cvar_t	*password;
extern	cvar_t	*spectator_password;
extern	cvar_t	*needpass;
extern	cvar_t	*g_select_empty;
extern	cvar_t	*dedicated;

extern	cvar_t	*filterban;
extern	cvar_t	*flashlightmode; //QW/ mode for flashlight code.
extern	cvar_t	*g_allowgrapple; //QW// allows grapple use

extern	cvar_t	*sv_gravity;
extern	cvar_t	*sv_maxvelocity;

extern	cvar_t	*gun_x, *gun_y, *gun_z;
extern	cvar_t	*sv_rollspeed;
extern	cvar_t	*sv_rollangle;

extern	cvar_t	*run_pitch;
extern	cvar_t	*run_roll;
extern	cvar_t	*bob_up;
extern	cvar_t	*bob_pitch;
extern	cvar_t	*bob_roll;

extern	cvar_t	*sv_cheats;
extern	cvar_t	*maxclients;
extern	cvar_t	*maxspectators;

extern	cvar_t	*flood_msgs;
extern	cvar_t	*flood_persecond;
extern	cvar_t	*flood_waitdelay;

extern	cvar_t	*sv_maplist;
extern	cvar_t	*jump_max_charge;
extern	cvar_t	*blaster_drop;
extern	cvar_t	*dm_items;
extern	cvar_t	*item_mod;
extern	cvar_t	*skyblend_minimum;
extern	cvar_t	*skyblend_maximum;
extern	cvar_t	*skyblend_speed;
extern	cvar_t	*sunblend;
extern	cvar_t	*real_shotgun_spread;
extern	cvar_t	*real_gibbing;
extern	cvar_t	*paranoia_mode;

#define world	(&g_edicts[0])

// item spawnflags
#define ITEM_TRIGGER_SPAWN		0x00000001
#define ITEM_NO_TOUCH			0x00000002
// 6 bits reserved for editor flags
// 8 bits used as power cube id bits for coop games
#define DROPPED_ITEM			0x00010000
#define	DROPPED_PLAYER_ITEM		0x00020000
#define ITEM_TARGETS_USED		0x00040000

//
// fields are needed for spawning from the entity string
// and saving / loading games
//
#define FFL_SPAWNTEMP		1
#define FFL_NOSPAWN			2

typedef enum {
	F_INT, 
	F_FLOAT,
	F_LSTRING,			// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,			// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_EDICT,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_FUNCTION,
	F_MMOVE,
	F_IGNORE,
	F_WXS, //extended weapon table weapon list
	F_WXT, //extended weapon table contents
} fieldtype_t;

typedef struct
{
	char	*name;
	int		ofs;
	fieldtype_t	type;
	int		flags;
} field_t;


extern	field_t fields[];
extern	gitem_t	itemlist[];



//
// g_cmds.c
//
void Cmd_Help_f (edict_t *ent);
void Cmd_Score_f (edict_t *ent);
void Cmd_pickup_depressed(edict_t* ent);

//
// g_items.c
//
void PrecacheItem (gitem_t *it);
void InitItems (void);
void SetItemNames (void);
gitem_t	*FindItem (char *pickup_name);
gitem_t	*FindItemByClassname (char *classname);
#define	ITEM_INDEX(x) ((x)-itemlist)
edict_t *Drop_Item (edict_t *ent, gitem_t *item);
void SetRespawn (edict_t *ent, float delay);
void ChangeWeapon (edict_t *ent);
void SpawnItem (edict_t *ent, gitem_t *item);
void Think_Weapon (edict_t *ent);
int ArmorIndex (edict_t *ent);
int PowerArmorType (edict_t *ent);
gitem_t	*GetItemByIndex (int index);
qboolean Add_Ammo (edict_t *ent, gitem_t *item, int count);
void Touch_Item (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf);

//
// g_utils.c
//

#if defined _WIN32
//
// Define a noreturn wrapper for gi.error
//
__declspec(noreturn) void GameError(char* fmt, ...);
#else
__attribute__((noreturn)) void GameError(char* fmt, ...);
#endif

qboolean	KillBox (edict_t *ent);
void	G_ProjectSource (vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
edict_t *G_Find (edict_t *from, int fieldofs, char *match);
edict_t *findradius (edict_t *from, vec3_t org, float rad);
edict_t *G_PickTarget (char *targetname);
void	G_UseTargets (edict_t *ent, edict_t *activator);
void	G_SetMovedir (vec3_t angles, vec3_t movedir);

void	G_InitEdict (edict_t *e);
edict_t	*G_Spawn (void);
void	G_FreeEdict (edict_t *e);

void	G_TouchTriggers (edict_t *ent);
void	G_TouchSolids (edict_t *ent);

char	*G_CopyString (const char *in);

float	*tv (float x, float y, float z);
char	*vtos (vec3_t v);

float vectoyaw (vec3_t vec);
void vectoangles (vec3_t vec, vec3_t angles);

//
// g_combat.c
//
qboolean OnSameTeam (edict_t *ent1, edict_t *ent2);
qboolean CanDamage (edict_t *targ, edict_t *inflictor);
void T_Damage (edict_t *targ, edict_t *inflictor, edict_t *attacker, vec3_t dir, vec3_t point, vec3_t normal, int damage, int knockback, int dflags, int mod);
void T_RadiusDamage (edict_t *inflictor, edict_t *attacker, float damage, edict_t *ignore, float radius, int mod);
qboolean check_knockback(edict_t* self);
void M_ReactToDamage(edict_t* targ, edict_t* attacker);
qboolean CheckTeamDamage(edict_t* targ, edict_t* attacker);

// damage flags
#define DAMAGE_RADIUS			0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR			0x00000002	// armour does not protect from this damage
#define DAMAGE_ENERGY			0x00000004	// damage is from an energy based weapon
#define DAMAGE_NO_KNOCKBACK		0x00000008	// do not affect velocity, just view angles
#define DAMAGE_BULLET			0x00000010  // damage is from a bullet (used for ricochets)
#define DAMAGE_NO_PROTECTION	0x00000020  // armor, shields, invulnerability, and godmode have no effect

#define DEFAULT_BULLET_HSPREAD	300
#define DEFAULT_BULLET_VSPREAD	500
#define DEFAULT_SHOTGUN_HSPREAD	1000
#define DEFAULT_SHOTGUN_VSPREAD	500
#define DEFAULT_DEATHMATCH_SHOTGUN_COUNT	12
#define DEFAULT_SHOTGUN_COUNT	12
#define DEFAULT_SSHOTGUN_COUNT	20

//
// g_monster.c
//
void monster_fire_bullet (edict_t *self, vec3_t start, vec3_t dir, int damage, int kick, int hspread, int vspread, int flashtype);
void monster_fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int flashtype);
void monster_fire_shotgun_grenade(edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype);
void monster_fire_blaster (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype, int effect);
void monster_fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int flashtype);
void monster_fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int flashtype);
void monster_fire_railgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype);
void monster_fire_bfg (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int kick, float damage_radius, int flashtype);
void M_droptofloor (edict_t *ent);
void monster_think (edict_t *self);
void walkmonster_start (edict_t *self);
void swimmonster_start (edict_t *self);
void flymonster_start (edict_t *self);
void AttackFinished (edict_t *self, float time);
void monster_death_use (edict_t *self);
void M_CatagorizePosition (edict_t *ent);
qboolean M_CheckAttack (edict_t *self);
void M_FlyCheck (edict_t *self);
void M_CheckGround (edict_t *ent);
void M_calcstrafepos(edict_t* self, vec3_t enemypos);
qboolean M_isrunningbackwards(edict_t* self);
qboolean M_isrunning(edict_t* self);

//
// g_misc.c
//
void ED_CallSpawn(edict_t* ent);
void ThrowHead (edict_t *self, char *gibname, int damage, int type);
void ThrowClientHead (edict_t *self, int damage);
void ThrowGib (edict_t *self, char *gibname, int damage, int type);
void BecomeExplosion1(edict_t *self);
void barrel_touch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void client_cmd(edict_t* ent, char* text);
void CTFGrappleFire(edict_t* ent, vec3_t g_offset, int damage, int effect);
void object_throw(edict_t* ent, int type);

//
// g_ai.c
//
void AI_SetSightClient (void);

void ai_stand (edict_t *self, float dist);
void ai_move (edict_t *self, float dist);
void ai_walk (edict_t *self, float dist);
void ai_turn (edict_t *self, float dist);
void ai_run (edict_t *self, float dist);
void ai_charge (edict_t *self, float dist);
int range (edict_t *self, edict_t *other);

void FoundTarget (edict_t *self);
qboolean infront (edict_t *self, edict_t *other);
qboolean visible (edict_t *self, edict_t *other);
qboolean FacingIdeal(edict_t *self);

//
// g_weapon.c
//
void ThrowDebris (edict_t *self, char *modelname, float speed, vec3_t origin, vec3_t dir, int spread);
qboolean fire_hit (edict_t *self, vec3_t aim, int damage, int kick);
void kick_hit (edict_t *self, int damage, int kick);
void fire_bullet (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod);
void fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod);
void fire_blaster (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, int effect, int type);
void fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius);
void fire_grenade2 (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held, int gravity);
void fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage);
void fire_rail (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick);
void fire_bfg (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius);
void fire_emp(edict_t* self, vec3_t start, vec3_t aimdir, int damage);
void fire_shotgun_grenade(edict_t* self, vec3_t start, vec3_t dir, int damage, int speed);

//
// g_ptrail.c
//
void PlayerTrail_Init (void);
void PlayerTrail_Add (vec3_t spot);
void PlayerTrail_New (vec3_t spot);
edict_t *PlayerTrail_PickFirst (edict_t *self);
edict_t *PlayerTrail_PickNext (edict_t *self);
edict_t	*PlayerTrail_LastSpot (void);

//
// g_client.c
//
void respawn (edict_t *ent);
void BeginIntermission (edict_t *targ);
void PutClientInServer (edict_t *ent);
void InitClientPersistant (gclient_t *client);
void InitClientResp (gclient_t *client);
void InitBodyQue (void);
void ClientBeginServerFrame (edict_t *ent);

//
// p_client.c
//
void player_setup(edict_t* ent);

//
// p_weapon.c
//
void Cmd_attack2a(edict_t* ent);
void Cmd_attack2b(edict_t* ent);
void Cmd_attack3a(edict_t* ent);
void Cmd_attack3b(edict_t* ent);
void Cmd_DualWielda(edict_t* ent);
void Cmd_DualWieldb(edict_t* ent);
void Cmd_Grapplea(edict_t* ent);
void Cmd_Grappleb(edict_t* ent);

//
// m_soldier.c
//
void soldier_stand3_skip(edict_t* self);

//
// g_player.c
//
void player_pain (edict_t *self, edict_t *other, float kick, int damage);
void player_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

//
// g_svcmds.c
//
void	ServerCommand (void);
qboolean SV_FilterPacket (char *from);

//
// p_view.c
//
void ClientEndServerFrame (edict_t *ent);

//
// p_hud.c
//
void MoveClientToIntermission (edict_t *client);
void G_SetStats (edict_t *ent);
void G_SetSpectatorStats (edict_t *ent);
void G_CheckChaseStats (edict_t *ent);
void ValidateSelectedItem (edict_t *ent);
void DeathmatchScoreboardMessage (edict_t *client, edict_t *killer);

//
// g_pweapon.c
//
void PlayerNoise(edict_t *who, vec3_t where, int type);

//
// m_move.c
//
qboolean M_CheckBottom (edict_t *ent);
qboolean M_walkmove (edict_t *ent, float yaw, float dist);
void M_MoveToGoal (edict_t *ent, float dist);
void M_ChangeYaw (edict_t *ent);

//
// g_phys.c
//
void G_RunEntity (edict_t *ent);

//
// g_main.c
//
void SaveClientData (void);
void FetchClientEntData (edict_t *ent);

//
// g_chase.c
//
void UpdateChaseCam(edict_t *ent);
void ChaseNext(edict_t *ent);
void ChasePrev(edict_t *ent);
void GetChaseTarget(edict_t *ent);

//============================================================================

// client_t->anim_priority
#define	ANIM_BASIC		0		// stand / run
#define	ANIM_WAVE		1
#define	ANIM_JUMP		2
#define	ANIM_PAIN		3
#define	ANIM_ATTACK		4
#define	ANIM_DEATH		5
#define	ANIM_REVERSE	6


// client data that stays across multiple level loads
typedef struct
{
	char		userinfo[MAX_INFO_STRING];
	char		netname[16];
	int			hand;

	qboolean	connected;			// a loadgame will leave valid entities that
									// just don't have a connection yet

	// values saved and restored from edicts when changing levels
	int			health;
	int			max_health;
	int			savedFlags;

	int			selected_item;
	int			inventory[MAX_ITEMS];

	// ammo capacities
	int			max_bullets;
	int			max_shells;
	int			max_rockets;
	int			max_grenades;
	int			max_cgrenades;
	int			max_cells;
	int			max_slugs;

	gitem_t		*weapon;
	gitem_t		*lastweapon;
	int			secondary; // to set the secondary fire switch
	int			scount;   // counter for secondary fire
	wext_t		weapon_ext; // current weapon extended table
	wexs_t		weapon_ext_save; //for saving extended table
	int			power_cubes;	// used for tracking the cubes in coop games
	int			score;			// for calculating total unit score in coop games

	int			game_helpchanged;
	int			helpchanged;

	qboolean	spectator;			// client is a spectator
	int			pickup; // object pickup state, like barrels, corpses etc.
	char		*view_modelb; //save here current view model when picking up objects
	int         jump_charge;
	int			items_activated; //to store items activated
	int			invu_health;
	int			quad_health;
	int			envi_health;
	int			rebr_health;
	int			sile_health;
	float       score_dmg_received;
	float		score_dmg_dealt;
	float		score_dmg_saved;
	float		score_kills;
	float		score_item_pickup;
	float		score_item_usage;
	float		score_health_bonus;
	float		score_objectives;
	float		score_secrets;
} client_persistant_t;

// client data that stays across deathmatch respawns
typedef struct
{
	client_persistant_t	coop_respawn;	// what to set client->pers to on a respawn
	int			enterframe;			// level.framenum the client entered the game
	int			score;				// frags, etc
	float       score_dmg_received;
	float		score_dmg_dealt;
	float		score_dmg_saved;
	float		score_kills;
	float		score_item_pickup;
	float		score_item_usage;
	float		score_health_bonus;
	float		score_objectives;
	float		score_secrets;
	float		score_total;
	vec3_t		cmd_angles;			// angles sent over in the last command

	qboolean	spectator;			// client is a spectator
} client_respawn_t;

// this structure is cleared on each PutClientInServer(),
// except for 'client->pers'
struct gclient_s
{
	// known to server
	player_state_t	ps;				// communicated by server to clients
	int				ping;

	// private to game
	client_persistant_t	pers;
	client_respawn_t	resp;
	pmove_state_t		old_pmove;	// for detecting out-of-pmove changes

	qboolean	showscores;			// set layout stat
	qboolean	showinventory;		// set layout stat
	qboolean	showhelp;
	qboolean	showhelpicon;

	int			ammo_index;

	int			buttons;
	int			oldbuttons;
	int			latched_buttons;
	int			buttonsx;
	qboolean	weapon_thunk;

	gitem_t		*newweapon;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_parmor;		// damage absorbed by power armor
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation

	float		killer_yaw;			// when dead, look at killer

	weaponstate_t	weaponstate;
	vec3_t		kick_angles;	// weapon kicks
	vec3_t		kick_origin;
	float		v_dmg_roll, v_dmg_pitch, v_dmg_yaw, v_dmg_time;	// damage kicks
	vec3_t		v_dmg_smooth; //to smooth out dmg punch
	float		fall_time, fall_value;		// for view drop on fall
	float		damage_alpha;
	float		bonus_alpha;
	vec3_t		damage_blend;
	vec3_t		v_angle;			// aiming direction
	float		bobtime;			// so off-ground doesn't change it
	vec3_t		oldviewangles;
	vec3_t		oldvelocity;

	float		next_drown_time;
	int			old_waterlevel;
	int			breather_sound;

	int			machinegun_shots;	// for weapon raising

	// animation vars
	int			anim_end;
	int			anim_priority;
	qboolean	anim_duck;
	qboolean	anim_run;

	// powerup timers
	float		quad_framenum;
	float		invincible_framenum;
	float		breather_framenum;
	float		enviro_framenum;

	qboolean	grenade_blew_up;
	float		grenade_time;
	int			silencer_shots;
	int			weapon_sound; 
	
	// flashlight
	edict_t*	flashlight;
	int		flashtype;

	float		pickup_msg_time;

	float		flood_locktill;		// locked from talking
	float		flood_when[10];		// when messages were said
	int			flood_whenhead;		// head pointer for when said

	float		respawn_time;		// can respawn when time > this

	edict_t		*chase_target;		// player we are chasing
	qboolean	update_chase;		// need to update chase info?
	int			kick; //KICKING STATE
	char			*hudguy; //icon
	float			hudguy_u;//when to freeze guys face
	float           hudguy_d; //accumulative damage
	edict_t     *grapple; //store grapple entity id 
	void		*ctf_grapple;		// entity of grapple
	int			ctf_grapplestate;		// true if pulling
	float		ctf_grapplereleasetime;	// time of grapple release
	float		ctf_grapplewindtimeout;	// time of grapple release
	//qboolean    got_powerscreen; //if already got power screen
	usercmd_t *ucmd; //save current ucmd
	usercmd_t *olducmd; //save current ucmd
	// ucmd stuff is messed up, I don't get it
	int forwardmove;
	int sidemove;
	int oldforwardmove;
	int oldsidemove;
};


struct edict_s
{
	entity_state_t	s;
	struct gclient_s	*client;	// NULL if not a player
									// the server expects the first part
									// of gclient_s to be a player_state_t
									// but the rest of it is opaque

	qboolean	inuse;
	int			linkcount;

	// FIXME: move these fields to a server private sv_entity_t
	link_t		area;				// linked to a division node or leaf
	
	int			num_clusters;		// if -1, use headnode instead
	int			clusternums[MAX_ENT_CLUSTERS];
	int			headnode;			// unused if num_clusters != -1
	int			areanum, areanum2;

	//================================

	int			svflags;
	vec3_t		mins, maxs;
	vec3_t		absmin, absmax, size;
	solid_t		solid;
	int			clipmask;
	edict_t		*owner;


	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!

	//================================
	int			movetype;
	int			flags;

	char		*model;
	float		freetime;			// sv.time when the object was freed
	
	//
	// only used locally in game, not by server
	//
	char		*message;
	char		*classname;
	int			spawnflags;

	float		timestamp;

	float		angle;			// set in qe3, -1 = up, -2 = down
	char		*target;
	char		*targetname;
	char		*killtarget;
	char		*team;
	char		*pathtarget;
	char		*deathtarget;
	char		*combattarget;
	edict_t		*target_ent;

	float		speed, accel, decel;
	vec3_t		movedir;
	vec3_t		pos1, pos2;

	vec3_t		velocity;
	vec3_t		avelocity;
	int			mass;
	float		air_finished;
	float		gravity;		// per entity gravity multiplier (1.0 is normal)
								// use for lowgrav artifact, flares

	edict_t		*goalentity;
	edict_t		*movetarget;
	float		yaw_speed;
	float		ideal_yaw;

	float		nextthink;
	void		(*prethink) (edict_t *ent);
	void		(*think)(edict_t *self);
	void		(*blocked)(edict_t *self, edict_t *other);	//move to moveinfo?
	void		(*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
	void		(*use)(edict_t *self, edict_t *other, edict_t *activator);
	void		(*pain)(edict_t *self, edict_t *other, float kick, int damage);
	void		(*die)(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);

	float		touch_debounce_time;		// are all these legit?  do we need more/less of them?
	float		pain_debounce_time;
	float		damage_debounce_time;
	float		fly_sound_debounce_time;	//move to clientinfo
	float		last_move_time;

	int			health;
	int			max_health;
	int			gib_health;
	int			deadflag;
	qboolean	show_hostile;

	float		powerarmor_time;

	char		*map;			// target_changelevel

	int			viewheight;		// height above origin where eyesight is determined
	int			takedamage;
	int			dmg;
	int			radius_dmg;
	float		dmg_radius;
	int			sounds;			//make this a spawntemp var?
	int			count;

	edict_t		*chain;
	edict_t		*enemy;
	edict_t		*oldenemy;
	edict_t		*activator;
	edict_t		*groundentity;
	int			groundentity_linkcount;
	edict_t		*teamchain;
	edict_t		*teammaster;

	edict_t		*mynoise;		// can go in client only
	edict_t		*mynoise2;

	int			noise_index;
	int			noise_index2;
	float		volume;
	float		attenuation;

	// timing variables
	float		wait;
	float		delay;			// before firing targets
	float		random;

	float		teleport_time;

	int			watertype;
	int			waterlevel;

	vec3_t		move_origin;
	vec3_t		move_angles;

	// move this to clientinfo?
	int			light_level;

	int			style;			// also used as areaportal number

	gitem_t		*item;			// for bonus items
	edict_t		*owner_solid; //to make projectiles collide with owner, we save the owner here and leave owner empty
	// common data blocks
	moveinfo_t		moveinfo;
	monsterinfo_t	monsterinfo;
	//woju
	edict_t *pickup_master;
	char		*musictrack;	// Knightmare- for specifying OGG or CD track
	int			bounce_amount; // when movetype = MOVETYPE_BOUNCE how much the entity will bounce
	float       gib_bleed_time; //life of gibs bleeding
	vec3_t      gib_bleed_dir; //direction of gibs bleeding
	int			mod; //means of death
	float skyblend;
	vec3_t	old_origin2; //we must use different vector, because this fucks up lerping
	float ctime; //when the entity was created :-)))
};

void P_ProjectSource(gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);
#define PICKUP_NONE				0	
#define PICKUP_ATTEMPT		1	// PICKUP BUTTON IS PRESSED
#define PICKUP_PICKINGUPSTART	2 //STARTED TO PICKING UP AN OBJECT
#define PICKUP_PICKINGUP		3		// PICKING UP OBJECT(INCLUDING HOLSTERING WEAPON)
#define PICKUP_PICKINGUPLAST	4 //FINISHED PICKING UP AN OBJECT
#define PICKUP_PICKEDUP			5	// PICKED UP SOMETHING
#define PICKUP_THROWING			6	// THROWING SOMETHING
#define PICKUP_OBJECT_BARREL 1
#define PICKUP_OBJECT_GUARD 2

extern char *vmodel_backup;

#define KICK_NONE 0
#define KICK_HOLSTER_START 1
#define KICK_HOLSTER 2
#define KICK_KICK 3
#define KICK_KICK2 4
#define KICK_LEG_DISTANCE 48
#define SECONDARY_BLASTER 1
#define SECONDARY_SHOTGUN 2
#define SECONDARY_SUPERSHOTGUN 4
#define SECONDARY_MACHINEGUN 8
#define SECONDARY_CHAINGUN 16
#define SECONDARY_CHAINGUN_DECCELERATE 32
#define SECONDARY_GRENADELAUNCHER 64
#define SECONDARY_ROCKETLAUNCHER 128
#define SECONDARY_HYPERBLASTER 256
#define SECONDARY_RAILGUN 512
#define SECONDARY_BFG 1024
#define SECONDARY_GRENADES 2048
#define BLASTER_MAX_CHARGE 50
//#define BLASTER_CHARGE_CALC 
int blaster_charge_calc(edict_t *ent, int charge);
#define SHOTGUN_GRENADE_SPEED 2500
#define SSHOTGUN_ALTFIRE_COUNT 4
#define IL_BLASTER 61
#define IL_SHOTGUN 31
#define LF_SUPERSHOTGUN 17
#define LF_MACHINEGUN 5
#define LF_CHAINGUN 31
#define LF_BFG 32
#define LF_HYPERBLASTER 20
#define LF_ROCKETLAUNCHER 12
#define BOLT_NOISY 1
#define BOLT_SILENT 2
#define BOLT_NOISY_HB 3
#define BOLT_NOISY2_HB 4
#define BOLT_NOISY2_HB_SLOW 5
#define BOLT_SILENT_HB 6
#define WEAPON_MODE_HYPERBLASTER_NORMAL 0
#define WEAPON_MODE_HYPERBLASTER_FAST 1
#define WEAPON_MODE_HYPERBLASTER_SLOW 2

#define WEAPON_MODE_BFG_NORMAL 0
#define WEAPON_MODE_BFG_FAST 1


typedef enum
{
	WEAPON_DUAL_CANT,
	WEAPON_DUAL_HASNT,
	WEAPON_DUAL_HAS,			
	WEAPON_DUAL_ACTIVE,
	WEAPON_DUAL_DEACTIVATING
} dualwield_t;

#define WEAPON_RAILGUN_FLAKCANNON_SPEED 5000
#define HB_DIR_NUM 30
#define WEAPON_MODE_GRENADE_LAUNCHER_NORMAL 0
#define WEAPON_MODE_GRENADE_LAUNCHER_CLUSTER 1
void toggle_gl_ammo(edict_t *ent);
#define WEAPON_MODE_CHAINGUN_DECCELERATING 0
#define WEAPON_MODE_CHAINGUN_ACCELERATING 1
#define WEAPON_MODE_GRENADE_NORMAL 0 
#define WEAPON_MODE_GRENADE_GRAVITY 1
#define WEAPON_MODE_GRENADE_TRAP 3

typedef enum
{
	BOUNCE_NORMAL,
	BOUNCE_LOWER,
	BOUNCE_LOWEST,
	BOUNCE_LOW,
	BOUNCE_HIGH,
	BOUNCE_HIGHER
}bounce_type_t;

//void gi.sound(edict_t *ent, int channel, int soundindex, float volume, float attenuation, float timeofs);
void reverse_hand(edict_t *ent);

#define GRENADE_FIRST			0x00000001
#define GRENADE_SECOND			0x00000002	
#define HUD_GUY_STAY_TIME 0.25
#define VectorEmpty(v) ((v)[0]==0&&(v)[1]==0&&(v)[2]==0)
void gib_target(edict_t *self, int damage, int type, vec3_t point);
void ThrowGib_exp(edict_t *self, char *gibname, int damage, int type, vec3_t point);
void ThrowHead_exp(edict_t *self, char *gibname, int damage, int type);
#define GIB_MECH				0x00000001
#define GIB_ORG					0x00000002
#define GIB_SMA					0x00000004
#define GIB_MED					0x00000008
#define GIB_BIG					0x00000010
#define GIB_PLAYER				0x00000020
#define GIB_FLYER				0x00000040
#define GIB_NOCHEST				0x00000080
#define GIB_NOBLOOD				0x00000100

//#define JUMP_YELLOW_CHARGE (jump_max_charge - (jump_max_charge/3))
//#define JUMP_RED_CHARGE (jump_max_charge - ((jump_max_charge/3)*2))
float clamp(float x, float upper, float lower);
#define JUMP_DAMP_MIN_VOL 0.05f
#define EMP_STUN_ADD 5

#define CTF_GRAPPLE_SPEED					1200 // speed of grapple in flight
#define CTF_GRAPPLE_PULL_SPEED				200	// speed player is pulled at
#define CTF_GRAPPLE_WIND_SPEED				400	// speed player is pulled at

// GRAPPLE
void CTFWeapon_Grapple(edict_t *ent);
void CTFPlayerResetGrapple(edict_t *ent);
void CTFGrapplePull(edict_t *self);
void CTFResetGrapple(edict_t *self);

typedef enum {
	CTF_GRAPPLE_STATE_OFF,
	CTF_GRAPPLE_STATE_HOLSTER,
	CTF_GRAPPLE_STATE_ACTIVATE_WEAPON,
	CTF_GRAPPLE_STATE_ACTIVATING,
	CTF_GRAPPLE_STATE_DEACTIVATING,
	CTF_GRAPPLE_STATE_FLY,
	CTF_GRAPPLE_STATE_PULL,
	CTF_GRAPPLE_STATE_WIND,
	CTF_GRAPPLE_STATE_HANG,

} ctfgrapplestate_t;

#define FRAME_AL_GRAPPLE 8
#define FRAME_FF_GRAPPLE 9
#define FRAME_IF_GRAPPLE 13
#define FRAME_IL_GRAPPLE 49
#define FRAME_DL_GRAPPLE 53
#define GRAPPLE_V_MODEL "models/weapons/grapple/tris.md2"
void GrappleWind(edict_t *self, vec3_t offset);
#define GRAPPLE_MAX_SPEED 1200
void add_gravity(edict_t *ent);
void CTFGrappleDrawCable(edict_t *self);
#define GRAPPLED_MAX_SPEED 400
#define GRAPPLE_WIND_TIMEOUT 3
void fix_angles(edict_t *self);
void spawn_blood(edict_t *self);
void spawn_blood_dir(edict_t *self, vec3_t dir);
void VectorScale2(vec3_t in, float scale, vec3_t out);
#define EMP_BACK_OFF 32
#define ENTITY_JANITOR_TIME 15
void entity_janitor(edict_t *ent);
void give_ammo(edict_t *ent, char *ammoname, int ammount);
void give_item(edict_t *ent, char *itemname);
#define EMP_CHARGE_TIME 15
#define EMP_CELL_COST 10
#define EMP_RECHARGE_TIME 5
#define GMF_ENHANCED_SAVEGAMES      0x00000400
#define G_FEATURES (GMF_ENHANCED_SAVEGAMES)
qboolean M_CheckClearShot(edict_t *self);

void debug_trail(vec3_t start, vec3_t end);
#define MONSTER_GUARD_BOLT_SPEED 1200
void predict_shot(edict_t *self, vec3_t origin, float proj_speed, vec3_t end, int type);
#define DODGE_CHECK_NUM 10
void check_dodge(edict_t *self, vec3_t start, vec3_t dir_org, int speed, int priority);
#define DODGE_PRIORITY_LOW 0
#define DODGE_PRIORITY_HIGH 1
void soldier_fire(edict_t *self, int flash_number);
void InfantryMachineGun(edict_t *self);
void floater_fire_blaster(edict_t *self);
void spawn_shockwave(edict_t *inflictor, edict_t *attacker, int damage, int radius, int mod);
qboolean CanDamageSphere(vec3_t start, edict_t *targ, float radius);
extern edict_t *DEBRIS_EDICT;
extern vec3_t DEBRIS_VELOCITY;
extern int DEBRIS_EXISTS;
void spawn_bubble(edict_t *self, vec3_t start);
#define REBREATHER_ACTIVATE_TIME 2
#define ENVIRO_ACTIVATE_TIME 3
void spawn_m_muzzleflash(edict_t *self, vec3_t start, vec3_t dir, int flashtype);
extern float enemy_yaw;
qboolean infront_point(edict_t *self, vec3_t point);
qboolean visible_point(edict_t *self, edict_t *other, vec3_t point);
//void monster_fire_railgun_flak(edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int flashtype);
float get_dist(edict_t *self, edict_t *other);
void monster_skip_frame(edict_t *self);
void add_sp_score(edict_t *self, int amount, double type);
#define SCORE_DAMAGE_RECEIVED -1
#define SCORE_DAMAGE_DEALT 2
#define SCORE_DAMAGE_SAVED 0.50001
#define	SCORE_KILLS 0.50002
#define SCORE_ITEM_PICKUP 1.00001
#define SCORE_ITEM_USAGE 1.00002
#define SCORE_ITEM_HEALTH_BONUS 4
#define SCORE_OBJECTIVES 1.00003
#define SCORE_SECRETS 1.00004
void IntermissionScoreboardMessage(edict_t *ent);
#define STRING_GREEN 1
#define STRING_WHITE 0
#define SHOW_DMG_RECEIVED		0x00000001
#define SHOW_DMG_DEALT			0x00000002
#define SHOW_DMG_SAVED			0x00000004
#define SHOW_KILLS				0x00000008
#define SHOW_ITEM_PICKUP		0x00000010
#define SHOW_ITEM_USAGE			0x00000020
#define SHOW_HEALTH_BONUS		0x00000040
#define SHOW_OBJECTIVES			0x00000080
#define SHOW_SECRETS			0x00000100
#define SHOW_TOTAL				0x00000200

void create_bloodsplat(edict_t* ent);
qboolean ffire_radius_check(edict_t *self);
qboolean visible_shootable(edict_t *self, edict_t *other);
void ThrowGib_damage(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
float location_scaling(edict_t *targ, vec3_t point, float damage, int  mod, int headshot);
#define INFANTRY_CLIPSIZE 50
void check_move_dir(edict_t *self, vec3_t point);

#define MOVE_LEFT 0	
#define MOVE_STRAIGHT 1
#define MOVE_RIGHT 2

void monster_jump(edict_t *self);

typedef enum {
	SCAN_LEFT,
	SCAN_RIGHT,
	SCAN_FORWARD,
	SCAN_BACKWARDS,
	SCAN_UP,
	SCAN_DOWN

} scandir_t;

float scan_dir(edict_t *self, int dir, float dist, vec3_t result);
#define DODGE_CHANCE 1
void throw_nuke(edict_t *self);
void M_avoid_danger(edict_t *self);
void spawn_blood_point(vec3_t point);
int next_spawn_is_gib;

#define MODE_SOLID 0
#define MODE_GHOST 1
#define BOLT_SIZE 2
#define BOLT_DAMAGE 0
#define BOLT_DESTROY 1
#define FLAK_SPREAD_VERTICAL 256	
#define FLAK_SPREAD_HORIZONTAL 256
void head_diepain(edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point);
void droptofloorx(edict_t *self);

float diff(float  a, float  b);	
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
qboolean infront_aiming(edict_t *self, edict_t *other);
float range_units(edict_t *self, edict_t *other);
float get_angledifference(edict_t *ent, float angle);
void faster_pain(edict_t *ent);
#define DODGE_FASTER_PAIN 3
#define BASE_YAW_SPEED 2500.0f

typedef enum
{
	MONSTER_BERSERKER,
	MONSTER_JORG,
	MONSTER_RIDER,
	MONSTER_BRAIN,
	MONSTER_CHICK,
	MONSTER_FLIPPER,
	MONSTER_FLOAT,
	MONSTER_FLYER,
	MONSTER_GLADIATOR,
	MONSTER_GUNNER,
	MONSTER_HOVER,
	MONSTER_INFANTRY,
	MONSTER_INSANE,
	MONSTER_MEDIC,
	MONSTER_MUTANT,
	MONSTER_PARASITE,
	MONSTER_SOLDIER,
	MONSTER_SUPERTANK,
	MONSTER_TANK
} monster_t;

void spawn_explosion(vec3_t origin, int type);
qboolean inbetw(int in, float down, float up);
void M_retreat(edict_t *self);
#define TRACE_LENGTH 8192
#define MONSTER_RETREAT_DEBOUNCE 10
float get_dist_point(vec3_t point1, vec3_t point2);
#define MONSTER_RETREAT_TIMEOUT 15
#define ANGLE_FORWARD 0
#define ANGLE_BACKWARDS 180
#define ANGLE_RIGHT 90
#define ANGLE_LEFT 270
#define RETREAT_SCAN_STEP 32
#define RETREAT_MAX_STEP_NUM 64
float get_dist2d(edict_t *self, edict_t *other);
float get_dist_v(edict_t *self, edict_t *other);
#define PREDICT_SHOT_MIN_DIST 256
float frame_mult();
#define MONSTER_STRAFE_MAX_TIME_DETECTION 1.0
#define MUZZLEFLASH_LEAD_MODEL "models/objects/mflash/tris.md2"
#define MUZZLEFLASH_BLASTER_MODEL "models/objects/explode/tris.md2"
#define SOLDIER_SHOTGUN_FIRE_DELAY 2
#define SOLDIER_BLASTER_FIRE_DELAY 0.4
#define DEAD_DMG_REDUCTION 0.2f
#define MAX_TEMP_INACCURACY_MULTIPLIER 2
#define TEMP_INACCURACY_DIVIDER 0.05
qboolean check_frames(edict_t *self, int start_frame, int end_frame);
extern int Sol_death5_start;
extern int Sol_death5_end;
#define MONSTER_RADIUSDMGAVOID_RADIUS 192
float get_dist2d_point(vec3_t point1, vec3_t point2);
#define FLAK_SPREAD 4
int	sound_alarm_debug;
#define TYPE_HITSCAN 0
#define TYPE_GRENADE 1
#define TYPE_PROJECTILE 2
#define TYPE_BLASTER 3
#define BLASTER_DROP_CONST 50
#define MIN_RETREAT_TIME 2

