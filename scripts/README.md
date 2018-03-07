# Midnight Commander plugin howto

After install, `parts` python script will be copied into `/usr/lib/mc/extfs.d`. Unfortunatelly this not enough for `mc` to figure out how to handle parts files.
So you have to add the following lines to you `$HOME/.config/mc/mc.ext` file:

```
# parts acrhive
shell/i/.parts
        Open=%cd %p/parts://
        View=%view{ascii} /usr/bin/parts_extract -l "${MC_EXT_FILENAME}"
```
