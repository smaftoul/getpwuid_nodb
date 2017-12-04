/* gcc -o getpwuid_nodb.so -fPIC -shared getpwuid_nodb.c -ldl
 * export LD_PRELOAD=$PWD/getpwuid_nodb.so
 * FAKE_USER="Dennis_Ritchie" whoami
 * docker run -u 1000:1000 -v $(pwd)/getpwuid_nodb.so:/tmp/getpwuid_nodb.so -e LD_PRELOAD=/tmp/getpwuid_nodb.so -e FAKE_USER="Dennis_Ritchie" alpine whoami
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

struct passwd *getpwuid(uid_t uid)
{
  static void * (*func)() = NULL;
  static char *username = NULL;
  struct passwd *p = NULL;

  if(!func)
  {
    func = (void *(*)()) dlsym(RTLD_NEXT, "getpwuid");
    username = getenv("FAKE_USER");
    if(username==NULL)
      username = "user";
  }

  p=func(0);
  if(p){
    p->pw_uid = getuid();
    p->pw_gid = getgid();
    p->pw_dir = "/tmp"; 
    p->pw_gecos = username; 
    p->pw_name = username;
  } 
  return(p);
}
