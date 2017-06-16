//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.cpp
//

#include "extdll.h"
#include "util.h"


#include "bot.h"

#ifndef __linux__
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif


skin_t bot_skins[MAX_SKINS];
int number_skins;

#define VALVE_MAX_SKINS     10
#define GEARBOX_MAX_SKINS   20
#define HOLYWARS_MAX_SKINS   5
#define CONFORCE_MAX_SKINS   3
#define SVEN_MAX_SKINS		59

extern int mod_id;

// default player models for various MODs...
char *valve_bot_models[VALVE_MAX_SKINS] = {
   "barney", "gina", "gman", "gordon", "helmet",
   "hgrunt", "recon", "robo", "scientist", "zombie"};

char *gearbox_bot_models[GEARBOX_MAX_SKINS] = {
   "barney", "beret", "cl_suit", "drill", "fassn", "gina", "gman",
   "gordon", "grunt", "helmet", "hgrunt", "massn", "otis", "recon",
   "recruit", "robo", "scientist", "shepard", "tower", "zombie"};

char *holywars_bot_models[HOLYWARS_MAX_SKINS] = {
   "akedo", "bad", "barney", "gordon", "helmet"};

char *conforce_bot_models[CONFORCE_MAX_SKINS] = {
   "jaguar", "shark", "wolf"};

char *sven_bot_models[SVEN_MAX_SKINS] = {
	"aswat", "barney", "barniel", "beret", "betagordon", "boris",
	"cannibal", "cl_suit", "colette", "dm_barney", "dm_gina",
	"dm_gordon", "dm_helmet", "etac", "fassn", "gina", "gman",
	"gordon", "grunt", "helmet", "hevbarney", "hevbarney2",
	"hevscientist", "hevscientist2", "hevscientist3",
	"hevscientist4", "hevscientist5", "junctionpunch",
	"kate", "madscientist", "mandalorian", "massn", "obi09", "otis",
	"punisher", "recon", "rgrunt", "robo", "scientist", "selene",
	"shephard", "seini", "th_cl_suit", "th_dave", "th_einar",
	"th_gangster", "th_jack", "th_neil", "th_nurse", "th_nypdcop",
	"th_orderly", "th_patient", "th_paul", "th_worker", "tower",
	"trinityrage", "unm_barney", "uscm", "zombie"};

// default names for each of the above player models...
char *valve_bot_names[VALVE_MAX_SKINS] = {
   "Barney", "Gina", "G-Man", "Gordon", "Helmet",
   "H-Grunt", "Recon", "Robo", "Scientist", "Zombie"};

char *gearbox_bot_names[GEARBOX_MAX_SKINS] = {
   "Barney", "Beret", "Cl_suit", "Drill", "Fassn", "Gina", "G-Man",
   "Gordon", "Grunt", "Helmet", "H-Grunt", "Massn", "Otis", "Recon",
   "Recruit", "Robo", "Scientist", "Shepard", "Tower", "Zombie"};

char *holywars_bot_names[HOLYWARS_MAX_SKINS] = {
   "Akedo", "B.A.D.", "Barney", "Gordon", "Helmet"};

char *conforce_bot_names[CONFORCE_MAX_SKINS] = {
   "Jaguar", "Shark", "Wolf"};

char *sven_bot_names[SVEN_MAX_SKINS] = {
	"Aswat", "Barney", "Barniel", "Beret", "Gordon-Beta", "Boris",
	"Cannibal", "CL-Suit", "Colette", "Barney-DM", "Gina-DM",
	"Gordon-DM", "Helmet-DM", "Etac", "Female-Assassin", "Gina", "Gman",
	"Gordon", "Grunt", "Helmet", "Barney-HEV", "Barney-HEV2",
	"Scientist-HEV", "Scientist-HEV2", "Scientist-HEV3",
	"Scientist-HEV4", "Scientist-HEV5", "Junction-Punch",
	"Kate", "Mad-Scientist", "Mandalorian", "Male-Assassin", "Obi09", "Otis",
	"Punisher", "Recon", "Robo-Grunt", "Robo", "Scientist", "Selene",
	"Shephard", "Seini", "TH-CL-Suit", "Dave", "Einar",
	"Gangster", "Jack", "Neil", "Nurse", "NYPD Cop",
	"Orderly", "Patient", "Paul", "Worker", "Tower",
	"Trinity-Rage", "Unmasked-Barney", "USCM", "Zombie"};

#ifndef __linux__

// MS-DOS directory wildcard routines...

HANDLE FindDirectory(HANDLE hFile, char *dirname, char *dirspec)
{
   WIN32_FIND_DATA pFindFileData;

   dirname[0] = 0;

   if (hFile == NULL)
   {
      hFile = FindFirstFile(dirspec, &pFindFileData);

      if (hFile == INVALID_HANDLE_VALUE)
         hFile = NULL;

      while (pFindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
      {
         if (FindNextFile(hFile, &pFindFileData) == 0)
         {
            FindClose(hFile);
            hFile = NULL;
            return hFile;
         }
      }

      strcpy(dirname, pFindFileData.cFileName);

      return hFile;
   }
   else
   {
      if (FindNextFile(hFile, &pFindFileData) == 0)
      {
         FindClose(hFile);
         hFile = NULL;
         return hFile;
      }

      while (pFindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
      {
         if (FindNextFile(hFile, &pFindFileData) == 0)
         {
            FindClose(hFile);
            hFile = NULL;
            return hFile;
         }
      }

      strcpy(dirname, pFindFileData.cFileName);

      return hFile;
   }
}

#else

// Linux directory wildcard routines...

DIR *FindDirectory(DIR *directory, char *dirname, char *dirspec)
{
   char pathname[256];
   struct dirent *dirent;
   struct stat stat_str;

   if (directory == NULL)
   {
      if ((directory = opendir(dirspec)) == NULL)
         return NULL;
   }

   while (1)
   {
      dirent = readdir(directory);

      if (dirent == NULL)  /* at end of directory? */
      {
         closedir(directory);
         return NULL;
      }

      strcpy(pathname, dirspec);
      strcat(pathname, "/");
      strcat(pathname, dirent->d_name);

      if (stat(pathname, &stat_str) == 0)
      {
         if (stat_str.st_mode & S_IFDIR)
         {
            strcpy(dirname, dirent->d_name);
            return directory;
         }
      }
   }
}

#endif


void LoadBotModels(void)
{
   char game_dir[256];
   char path[MAX_PATH];
   char search_path[MAX_PATH];
   char dirname[MAX_PATH];
   char filename[MAX_PATH];
   int index;
   struct stat stat_str;
#ifndef __linux__
   HANDLE directory = NULL;
#else
   DIR *directory = NULL;
#endif

   for (index=0; index < MAX_SKINS; index++)
      bot_skins[index].skin_used = FALSE;

   if ((mod_id == VALVE_DLL) || (mod_id == DMC_DLL))
   {
	   number_skins = VALVE_MAX_SKINS;

      for (index=0; index < VALVE_MAX_SKINS; index++)
      {
         strcpy(bot_skins[index].model_name, valve_bot_models[index]);
         strcpy(bot_skins[index].bot_name, valve_bot_names[index]);
      }
   }
   else if (mod_id == GEARBOX_DLL)
   {
	   number_skins = GEARBOX_MAX_SKINS;

      for (index=0; index < GEARBOX_MAX_SKINS; index++)
      {
         strcpy(bot_skins[index].model_name, gearbox_bot_models[index]);
         strcpy(bot_skins[index].bot_name, gearbox_bot_names[index]);
      }
   }
   else if (mod_id == HOLYWARS_DLL)
   {
	   number_skins = HOLYWARS_MAX_SKINS;

      for (index=0; index < HOLYWARS_MAX_SKINS; index++)
      {
         strcpy(bot_skins[index].model_name, holywars_bot_models[index]);
         strcpy(bot_skins[index].bot_name, holywars_bot_names[index]);
      }
   }
   else if (mod_id == CONFORCE_DLL)
   {
	   number_skins = CONFORCE_MAX_SKINS;

      for (index=0; index < CONFORCE_MAX_SKINS; index++)
      {
         strcpy(bot_skins[index].model_name, conforce_bot_models[index]);
         strcpy(bot_skins[index].bot_name, conforce_bot_names[index]);
      }
   }
   else if (mod_id == SVEN_DLL)
   {
	   number_skins = SVEN_MAX_SKINS;

      for (index=0; index < SVEN_MAX_SKINS; index++)
      {
         strcpy(bot_skins[index].model_name, sven_bot_models[index]);
         strcpy(bot_skins[index].bot_name, sven_bot_names[index]);
      }
   }

   // find the directory name of the currently running MOD...
   (*g_engfuncs.pfnGetGameDir)(game_dir);

   strcpy(path, game_dir);
   strcat(path, "/models/player");

   UTIL_Pathname_Convert(path);

   if (stat(path, &stat_str) != 0)
   {
      // use the valve/models/player directory if no MOD models/player
      strcpy(path, "valve/models/player");
   }

   UTIL_Pathname_Convert(path);

   strcpy(search_path, path);

#ifndef __linux__
   strcat(search_path, "/*");
#endif

   UTIL_Pathname_Convert(search_path);

   while ((directory = FindDirectory(directory, dirname, search_path)) != NULL)
   {
      if ((strcmp(dirname, ".") == 0) || (strcmp(dirname, "..") == 0))
         continue;

      strcpy(filename, path);
      strcat(filename, "/");
      strcat(filename, dirname);
      strcat(filename, "/");
      strcat(filename, dirname);
      strcat(filename, ".mdl");

      UTIL_Pathname_Convert(filename);

      if (stat(filename, &stat_str) == 0)
      {
         // add this model to the array of models...
         for (index=0; dirname[index]; index++)
            dirname[index] = tolower(dirname[index]);

         // check for duplicate...
         for (index=0; index < number_skins; index++)
         {
            if (strcmp(dirname, bot_skins[index].model_name) == 0)
               break;
         }

         if (index == number_skins)
         {
            // add this model to the bot_skins array...
            strcpy(bot_skins[number_skins].model_name, dirname);

            dirname[0] = toupper(dirname[0]);
            strcpy(bot_skins[number_skins].bot_name, dirname);

            number_skins++;
         }
      }

      if (number_skins == MAX_SKINS)
         break;  // break out if max models reached
   }
}

