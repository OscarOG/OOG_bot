//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// h_export.cpp
//

#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"

#include "bot.h"
#include "engine.h"

#ifndef __linux__

HINSTANCE h_Library = NULL;
HGLOBAL h_global_argv = NULL;
void FreeNameFuncGlobals(void);
void LoadSymbols(char *filename);

#else

void *h_Library = NULL;
char h_global_argv[1024];

#endif

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
char *g_argv;

static FILE *fp;
char z_welcome_msg[] = "HPB bot - OOG";


GETENTITYAPI other_GetEntityAPI = NULL;
GETNEWDLLFUNCTIONS other_GetNewDLLFunctions = NULL;
GIVEFNPTRSTODLL other_GiveFnptrsToDll = NULL;

extern int mod_id;


#ifndef __linux__

// Required DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   if (fdwReason == DLL_PROCESS_ATTACH)
   {
   }
   else if (fdwReason == DLL_PROCESS_DETACH)
   {
      FreeNameFuncGlobals();  // Free exported symbol table

      if (h_Library)
         FreeLibrary(h_Library);

      if (h_global_argv)
      {
         GlobalUnlock(h_global_argv);
         GlobalFree(h_global_argv);
      }
   }

   return TRUE;
}

#endif

#ifndef __linux__
#ifdef __BORLANDC__
extern "C" DLLEXPORT void EXPORT GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals)
#else
void DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#endif
#else
extern "C" DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#endif
{
   int pos;
   char game_dir[256];
   char mod_name[32];
   char game_dll_filename[256];

   // get the engine functions from the engine...

   memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
   gpGlobals = pGlobals;

   // find the directory name of the currently running MOD...
   (*g_engfuncs.pfnGetGameDir)(game_dir);

   pos = 0;

   if (strstr(game_dir, "/") != NULL)
   {
      pos = strlen(game_dir) - 1;

      // scan backwards till first directory separator...
      while ((pos) && (game_dir[pos] != '/'))
         pos--;

      if (pos == 0)
      {
         // Error getting directory name!

   		ALERT( at_error, "HPB_bot - Error determining MOD directory name!" );
      }

      pos++;
   }

   strcpy(mod_name, &game_dir[pos]);

   game_dll_filename[0] = 0;

   if (strcmpi(mod_name, "valve") == 0)
   {
      mod_id = VALVE_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "valve\\dlls\\hl.dll");
#else
      strcpy(game_dll_filename, "valve/dlls/hl_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "tfc") == 0)
   {
      mod_id = TFC_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "tfc\\dlls\\tfc.dll");
#else
      strcpy(game_dll_filename, "tfc/dlls/tfc_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "cstrike") == 0)
   {
      mod_id = CSTRIKE_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "cstrike\\dlls\\mp.dll");
#else
      strcpy(game_dll_filename, "cstrike/dlls/cs_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "gearbox") == 0)
   {
      mod_id = GEARBOX_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "gearbox\\dlls\\opfor.dll");
#else
      strcpy(game_dll_filename, "gearbox/dlls/opfor_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "frontline") == 0)
   {
      mod_id = FRONTLINE_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "frontline\\dlls\\frontline.dll");
#else
      strcpy(game_dll_filename, "frontline/dlls/front_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "holywars") == 0)
   {
      mod_id = HOLYWARS_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "holywars\\dlls\\holywars.dll");
#else
      strcpy(game_dll_filename, "holywars/dlls/holywars_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "dmc") == 0)
   {
      mod_id = DMC_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "dmc\\dlls\\dmc.dll");
#else
      strcpy(game_dll_filename, "dmc/dlls/dmc_i386.so");
#endif
   }
   else if (strcmpi(mod_name, "conforce") == 0)
   {
      mod_id = CONFORCE_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "conforce\\dlls\\cf.dll");
#endif
   }
   else if (strcmpi(mod_name, "brainbread") == 0)
   {
      mod_id = BB_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "brainbread\\dlls\\bb.dll");
#endif
   }
   else if (strcmpi(mod_name, "svencoop") == 0)
   {
      mod_id = SVEN_DLL;
#ifndef __linux__
      strcpy(game_dll_filename, "svencoop\\dlls\\hl.dll");
#endif
   }

   if (game_dll_filename[0])
   {
#ifndef __linux__
      h_Library = LoadLibrary(game_dll_filename);
#else
      h_Library = dlopen(game_dll_filename, RTLD_NOW);
#endif
   }
   else
   {
      // Directory error or Unsupported MOD!

		ALERT( at_error, "HPB_bot - MOD dll not found (or unsupported MOD)!" );

      return;
   }

#ifndef __linux__
   h_global_argv = GlobalAlloc(GMEM_SHARE, 1024);
   g_argv = (char *)GlobalLock(h_global_argv);
#else
   g_argv = (char *)h_global_argv;
#endif

   other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");

   if (other_GetEntityAPI == NULL)
   {
      // Can't find GetEntityAPI!

		ALERT( at_error, "HPB_bot - Can't get MOD's GetEntityAPI!" );
   }

   other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library, "GetNewDLLFunctions");

//   if (other_GetNewDLLFunctions == NULL)
//   {
//      // Can't find GetNewDLLFunctions!
//
//		ALERT( at_error, "HPB_bot - Can't get MOD's GetNewDLLFunctions!" );
//   }

   other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");

   if (other_GiveFnptrsToDll == NULL)
   {
      // Can't find GiveFnptrsToDll!

		ALERT( at_error, "HPB_bot - Can't get MOD's GiveFnptrsToDll!" );
   }


#ifndef __linux__
   LoadSymbols(game_dll_filename);  // Load exported symbol table
#endif

   pengfuncsFromEngine->pfnCmd_Args = Cmd_Args;
   pengfuncsFromEngine->pfnCmd_Argv = Cmd_Argv;
   pengfuncsFromEngine->pfnCmd_Argc = Cmd_Argc;

   pengfuncsFromEngine->pfnPrecacheModel = pfnPrecacheModel;
   pengfuncsFromEngine->pfnPrecacheSound = pfnPrecacheSound;
   pengfuncsFromEngine->pfnSetModel = pfnSetModel;
   pengfuncsFromEngine->pfnModelIndex = pfnModelIndex;
   pengfuncsFromEngine->pfnModelFrames = pfnModelFrames;
   pengfuncsFromEngine->pfnSetSize = pfnSetSize;
   pengfuncsFromEngine->pfnChangeLevel = pfnChangeLevel;
   pengfuncsFromEngine->pfnGetSpawnParms = pfnGetSpawnParms;
   pengfuncsFromEngine->pfnSaveSpawnParms = pfnSaveSpawnParms;
   pengfuncsFromEngine->pfnVecToYaw = pfnVecToYaw;
   pengfuncsFromEngine->pfnVecToAngles = pfnVecToAngles;
   pengfuncsFromEngine->pfnMoveToOrigin = pfnMoveToOrigin;
   pengfuncsFromEngine->pfnChangeYaw = pfnChangeYaw;
   pengfuncsFromEngine->pfnChangePitch = pfnChangePitch;
   pengfuncsFromEngine->pfnFindEntityByString = pfnFindEntityByString;
   pengfuncsFromEngine->pfnGetEntityIllum = pfnGetEntityIllum;
   pengfuncsFromEngine->pfnFindEntityInSphere = pfnFindEntityInSphere;
   pengfuncsFromEngine->pfnFindClientInPVS = pfnFindClientInPVS;
   pengfuncsFromEngine->pfnEntitiesInPVS = pfnEntitiesInPVS;
   pengfuncsFromEngine->pfnMakeVectors = pfnMakeVectors;
   pengfuncsFromEngine->pfnAngleVectors = pfnAngleVectors;
   pengfuncsFromEngine->pfnCreateEntity = pfnCreateEntity;		//	<--*
   pengfuncsFromEngine->pfnRemoveEntity = pfnRemoveEntity;		//	<--*
   pengfuncsFromEngine->pfnCreateNamedEntity = pfnCreateNamedEntity;	//	<--*
   pengfuncsFromEngine->pfnMakeStatic = pfnMakeStatic;
   pengfuncsFromEngine->pfnEntIsOnFloor = pfnEntIsOnFloor;
   pengfuncsFromEngine->pfnDropToFloor = pfnDropToFloor;
   pengfuncsFromEngine->pfnWalkMove = pfnWalkMove;
   pengfuncsFromEngine->pfnSetOrigin = pfnSetOrigin;
   pengfuncsFromEngine->pfnEmitSound = pfnEmitSound;
   pengfuncsFromEngine->pfnEmitAmbientSound = pfnEmitAmbientSound;
   pengfuncsFromEngine->pfnTraceLine = pfnTraceLine;
   pengfuncsFromEngine->pfnTraceToss = pfnTraceToss;
   pengfuncsFromEngine->pfnTraceMonsterHull = pfnTraceMonsterHull;
   pengfuncsFromEngine->pfnTraceHull = pfnTraceHull;
   pengfuncsFromEngine->pfnTraceModel = pfnTraceModel;
   pengfuncsFromEngine->pfnTraceTexture = pfnTraceTexture;
   pengfuncsFromEngine->pfnTraceSphere = pfnTraceSphere;
   pengfuncsFromEngine->pfnGetAimVector = pfnGetAimVector;
   pengfuncsFromEngine->pfnServerCommand = pfnServerCommand;
   pengfuncsFromEngine->pfnServerExecute = pfnServerExecute;
   pengfuncsFromEngine->pfnClientCommand = pfnClientCommand;
   pengfuncsFromEngine->pfnParticleEffect = pfnParticleEffect;
   pengfuncsFromEngine->pfnLightStyle = pfnLightStyle;
   pengfuncsFromEngine->pfnDecalIndex = pfnDecalIndex;
   pengfuncsFromEngine->pfnPointContents = pfnPointContents;
   pengfuncsFromEngine->pfnMessageBegin = pfnMessageBegin;
   pengfuncsFromEngine->pfnMessageEnd = pfnMessageEnd;
   pengfuncsFromEngine->pfnWriteByte = pfnWriteByte;
   pengfuncsFromEngine->pfnWriteChar = pfnWriteChar;
   pengfuncsFromEngine->pfnWriteShort = pfnWriteShort;
   pengfuncsFromEngine->pfnWriteLong = pfnWriteLong;
   pengfuncsFromEngine->pfnWriteAngle = pfnWriteAngle;
   pengfuncsFromEngine->pfnWriteCoord = pfnWriteCoord;
   pengfuncsFromEngine->pfnWriteString = pfnWriteString;
   pengfuncsFromEngine->pfnWriteEntity = pfnWriteEntity;
   pengfuncsFromEngine->pfnCVarRegister = pfnCVarRegister;
   pengfuncsFromEngine->pfnCVarGetFloat = pfnCVarGetFloat;
   pengfuncsFromEngine->pfnCVarGetString = pfnCVarGetString;
   pengfuncsFromEngine->pfnCVarSetFloat = pfnCVarSetFloat;
   pengfuncsFromEngine->pfnCVarSetString = pfnCVarSetString;
   pengfuncsFromEngine->pfnPvAllocEntPrivateData = pfnPvAllocEntPrivateData;
   pengfuncsFromEngine->pfnPvEntPrivateData = pfnPvEntPrivateData;
   pengfuncsFromEngine->pfnFreeEntPrivateData = pfnFreeEntPrivateData;
   pengfuncsFromEngine->pfnSzFromIndex = pfnSzFromIndex;
   pengfuncsFromEngine->pfnAllocString = pfnAllocString;
   pengfuncsFromEngine->pfnGetVarsOfEnt = pfnGetVarsOfEnt;
   pengfuncsFromEngine->pfnPEntityOfEntOffset = pfnPEntityOfEntOffset;
   pengfuncsFromEngine->pfnEntOffsetOfPEntity = pfnEntOffsetOfPEntity;
   pengfuncsFromEngine->pfnIndexOfEdict = pfnIndexOfEdict;
   pengfuncsFromEngine->pfnPEntityOfEntIndex = pfnPEntityOfEntIndex;
   pengfuncsFromEngine->pfnFindEntityByVars = pfnFindEntityByVars;
   pengfuncsFromEngine->pfnGetModelPtr = pfnGetModelPtr;
   pengfuncsFromEngine->pfnRegUserMsg = pfnRegUserMsg;
   pengfuncsFromEngine->pfnAnimationAutomove = pfnAnimationAutomove;
   pengfuncsFromEngine->pfnGetBonePosition = pfnGetBonePosition;
   pengfuncsFromEngine->pfnFunctionFromName = pfnFunctionFromName;
   pengfuncsFromEngine->pfnNameForFunction = pfnNameForFunction;
   pengfuncsFromEngine->pfnClientPrintf = pfnClientPrintf;
   pengfuncsFromEngine->pfnServerPrint = pfnServerPrint;
   pengfuncsFromEngine->pfnGetAttachment = pfnGetAttachment;
   pengfuncsFromEngine->pfnCRC32_Init = pfnCRC32_Init;
   pengfuncsFromEngine->pfnCRC32_ProcessBuffer = pfnCRC32_ProcessBuffer;
   pengfuncsFromEngine->pfnCRC32_ProcessByte = pfnCRC32_ProcessByte;
   pengfuncsFromEngine->pfnCRC32_Final = pfnCRC32_Final;
   pengfuncsFromEngine->pfnRandomLong = pfnRandomLong;
   pengfuncsFromEngine->pfnRandomFloat = pfnRandomFloat;
   pengfuncsFromEngine->pfnSetView = pfnSetView;
   pengfuncsFromEngine->pfnTime = pfnTime;
   pengfuncsFromEngine->pfnCrosshairAngle = pfnCrosshairAngle;
   pengfuncsFromEngine->pfnLoadFileForMe = pfnLoadFileForMe;
   pengfuncsFromEngine->pfnFreeFile = pfnFreeFile;
   pengfuncsFromEngine->pfnEndSection = pfnEndSection;
   pengfuncsFromEngine->pfnCompareFileTime = pfnCompareFileTime;
   pengfuncsFromEngine->pfnGetGameDir = pfnGetGameDir;
   pengfuncsFromEngine->pfnCvar_RegisterVariable = pfnCvar_RegisterVariable;
   pengfuncsFromEngine->pfnFadeClientVolume = pfnFadeClientVolume;
   pengfuncsFromEngine->pfnSetClientMaxspeed = pfnSetClientMaxspeed;
   pengfuncsFromEngine->pfnCreateFakeClient = pfnCreateFakeClient;
   pengfuncsFromEngine->pfnRunPlayerMove = pfnRunPlayerMove;
   pengfuncsFromEngine->pfnNumberOfEntities = pfnNumberOfEntities;
   pengfuncsFromEngine->pfnGetInfoKeyBuffer = pfnGetInfoKeyBuffer;
   pengfuncsFromEngine->pfnInfoKeyValue = pfnInfoKeyValue;
   pengfuncsFromEngine->pfnSetKeyValue = pfnSetKeyValue;
   pengfuncsFromEngine->pfnSetClientKeyValue = pfnSetClientKeyValue;
   pengfuncsFromEngine->pfnIsMapValid = pfnIsMapValid;
   pengfuncsFromEngine->pfnStaticDecal = pfnStaticDecal;
   pengfuncsFromEngine->pfnPrecacheGeneric = pfnPrecacheGeneric;
   pengfuncsFromEngine->pfnGetPlayerUserId = pfnGetPlayerUserId;
   pengfuncsFromEngine->pfnBuildSoundMsg = pfnBuildSoundMsg;
   pengfuncsFromEngine->pfnIsDedicatedServer = pfnIsDedicatedServer;
   pengfuncsFromEngine->pfnCVarGetPointer = pfnCVarGetPointer;
   pengfuncsFromEngine->pfnGetPlayerWONId = pfnGetPlayerWONId;

   // SDK 2.0 additions...
   pengfuncsFromEngine->pfnInfo_RemoveKey = pfnInfo_RemoveKey;
   pengfuncsFromEngine->pfnGetPhysicsKeyValue = pfnGetPhysicsKeyValue;
   pengfuncsFromEngine->pfnSetPhysicsKeyValue = pfnSetPhysicsKeyValue;
   pengfuncsFromEngine->pfnGetPhysicsInfoString = pfnGetPhysicsInfoString;
   pengfuncsFromEngine->pfnPrecacheEvent = pfnPrecacheEvent;
   pengfuncsFromEngine->pfnPlaybackEvent = pfnPlaybackEvent;
   pengfuncsFromEngine->pfnSetFatPVS = pfnSetFatPVS;
   pengfuncsFromEngine->pfnSetFatPAS = pfnSetFatPAS;
   pengfuncsFromEngine->pfnCheckVisibility = pfnCheckVisibility;
   pengfuncsFromEngine->pfnDeltaSetField = pfnDeltaSetField;
   pengfuncsFromEngine->pfnDeltaUnsetField = pfnDeltaUnsetField;
   pengfuncsFromEngine->pfnDeltaAddEncoder = pfnDeltaAddEncoder;
   pengfuncsFromEngine->pfnGetCurrentPlayer = pfnGetCurrentPlayer;
   pengfuncsFromEngine->pfnCanSkipPlayer = pfnCanSkipPlayer;
   pengfuncsFromEngine->pfnDeltaFindField = pfnDeltaFindField;
   pengfuncsFromEngine->pfnDeltaSetFieldByIndex = pfnDeltaSetFieldByIndex;
   pengfuncsFromEngine->pfnDeltaUnsetFieldByIndex = pfnDeltaUnsetFieldByIndex;
   pengfuncsFromEngine->pfnSetGroupMask = pfnSetGroupMask;
   pengfuncsFromEngine->pfnCreateInstancedBaseline = pfnCreateInstancedBaseline;
   pengfuncsFromEngine->pfnCvar_DirectSet = pfnCvar_DirectSet;
   pengfuncsFromEngine->pfnForceUnmodified = pfnForceUnmodified;
   pengfuncsFromEngine->pfnGetPlayerStats = pfnGetPlayerStats;
   pengfuncsFromEngine->pfnAddServerCommand = pfnAddServerCommand;

   // give the engine functions to the other DLL...
   (*other_GiveFnptrsToDll)(pengfuncsFromEngine, pGlobals);
}

