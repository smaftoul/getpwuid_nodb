getpwuid_nodb
=============

## Example

```
$ docker run -ti -v $(pwd):$(pwd) --workdir $(pwd) alpine touch foo
$ docker run -ti -v $(pwd):$(pwd) --workdir $(pwd) -u 10000:10000 alpine touch bar
$ ls -l
total 12
-rw-r--r-- 1  10000  10000    0 Dec  4 14:12 bar
-rw-r--r-- 1 root   root      0 Dec  4 14:12 foo
$ docker run -ti -v $(pwd):$(pwd) --workdir $(pwd) alpine getent passwd 10000
$ echo $?
2
$ docker run -ti -v $(pwd):$(pwd) --workdir $(pwd) -u 10000:10000 alpine getent passwd 10000
$ echo $?
2
$ docker run -ti -v $(pwd):$(pwd) --workdir $(pwd) -u 10000:10000 -v $(pwd)/getpwuid_nodb.so:/tmp/getpwuid_nodb.so -e LD_PRELOAD=/tmp/getpwuid_nodb.so -e FAKE_USER=Dennis_Ritchie alpine getent passwd 10000
Dennis_Ritchie:x:10000:10000:Dennis_Ritchie:/tmp:/bin/ash
```

## Explanation

When running a container, you can specifiy which uid will be running the container's entrypoint by specifying the `-u` argument.
As you can see in the above example, it will change the owner / group of files created in a bind mount / volume.

If you, for example, install vendors, this might be important, so your permissions are correct on your filesystem.

The problem with this is that the user id you are using, may not exist in your container's `/etc/passwd` which can create problems with different software, especially `ssh` which is often used with git to checkout private vendors.

This wrapper dynamically simulates the presence of a `/etc/passwd` entry for the current uid you are using. The username will be the value of the `FAKE_USER` environement variable if present, otherwise it will be `user`. The home of this fake user will be `/tmp`.

## Limitations

* the docker container doesn't have a `/etc/passwd` (we get uid 0 entry from it).
* the docker container have a `/etc/passwd` but doesn't have a uid 0 entry.
* it probably won't work a statically compiled binary