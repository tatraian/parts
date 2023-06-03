# parts archive format

`.parts` archive format is created for easy update of large binary data.

In contrast of several other compression formats (such as tar.* or zip), this format holds the Table of Contents of the archive in one block at the beginning. The files are also compressed individually. This produce a slightly larger archive size, but through network you are able to download only the table of content or only one (compressed) file.  
  
The library (and the command line tool also) has interface that downloads only the binary difference (per file) of a directory.


## Using compressor and extractor from command line

The compressor application can be used as usually a command line compressor application. For example with the next command you will be compress the parts source code
```
parts_compress parts_source.parts parts
```

With the next command you can extract it:
```
parts_extract parts_source.parts new_destination
```
The `parts_extract` command creates `new_destination` directory and put `parts` folder into that.
After we modify the `parts_extract.cpp` source file and we recompress the source directory again:

```
parts_compress parts_source-v2.parts parts
```

Now the `parts_extract` command will be able to extract `parts_source-v2.parts` archive with the help of `new_destination/parts` directory.

```
parts_extract parts_source-v2.parts new_destination/parts-v2 -u new_destination/parts
```

This command will extract the archive into `new_destination/parts-v2/parts` directory. In the log we can see the next
```
...
Copy file:    parts/src/parts_compress.cpp
Extract file: parts/src/parts_extract.cpp
Seeking position: 501401
Create dir:   parts/thirdparty
...
```

So `parts_compress.cpp` file is just copied from `new_destination/parts` directory and only the modified `parts_extract.cpp` is extracted from the archive file.

Of course, this doesn't have relevant in case of local files, but `parts_extract` can read the input archive from **http(s)** protocol too. In this case the matching files will be copied only on the local filesystem and only the difference (and the Table of content) will be downloaded from the server.

## Using parts as library
