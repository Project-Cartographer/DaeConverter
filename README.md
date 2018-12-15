# DaeConverter
A command line based app to import and extract Halo 2 Render Model tags

[![Build status](https://ci.appveyor.com/api/projects/status/bul7rrcbjeuk5qna/branch/master?svg=true)](https://ci.appveyor.com/project/Himanshu-01/daeconverter/branch/master)

## Launch Parameters ##

#### Compiling Collada models ####
  * Use "-compile <Model_Directory>"
  * Example :  DaeConverter.exe -compile BR
  * Note : The [<Model_Directory>](#model_directory) must be organised properly
  
#### Decompiling Render Model ####
  * Use "-decompile <render_model file>"
  * Example :  DaeConverter.exe -decompile BR\BR.render_model
  * Note : The Path is relative to CurrentDirectory
  
  #### Copying Node data ####
  * Use "-replace-node <destination render_model file> <source render_model file>"
  * Example : DaeConverter.exe -replace-node test\test.render_model test\battle_rifle.render_model
  * Note : The Path is relative to CurrentDirectory	
  
  #### Copying Marker_group data ####
  * similar to node data copying  
  * "-replace-marker <destination render_model file> <source render_model file>"
  
## Model_Directory
* __ParentFolder__/__Regions__/__Permutations__/_Collada Files_(.dae)
* Check [Example](https://github.com/Himanshu-01/DaeConverter/tree/master/Example)         
  
  
  
## Status ##
* __Completed__ 
  * the loading part of the render_model 21/2/18,a bit broken
  * render_model dumping function at 04/04/18
  * Supports multiple Permutations and multiple models
  * Supports Nodes
  * Supports Node copying

* __Incomplete__
	* collada dumping function
	* collada loading function(its in alpha stage,therefore not error free)  
  
## Contributing ##
Contributions are welcome, MSVC 2015 is currently used to build the app, and is therefore the recommended IDE.

### Compiling ###
* Clone and build the MSVC solution

## Acknowledgement ##
* Uses [Assimp Library](http://www.assimp.org/)

## Bug Reports ##
The Program is still incomplete and often breaks when trying to extract render data of some tags.
It is mainly due to different TAG_BLOCK header of certain tags.
