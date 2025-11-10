/*

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
----------------------------------------------main.h------------------------------------------

TODO: Split out into Core/PAL module
TODO: Split out into Input Handling module
TODO: Split out into UI module

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
----------------------------------------------main.cpp----------------------------------------

TODO: Get rid of runtime path discovery in release builds
TODO: per-thread string memory system, to be sized based on thread's need. Rendering thread 
      will be heaviest user

TODO: turn gltf processing into a module, define a standardized gltf format for game resources 
      and support only that. ideally, we call cgltf_free() immediately after doing this 
      processing

TODO: Switch to one buffer per level and using glBufferSubData/glDrawElementsBaseVertex - 
      eventually glDrawElementsIndirectCommand
      /!\ remember to make changes where the draw calls actually happen too

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
----------------------------------------------pal.h-------------------------------------------

TODO: Get rid of runtime path discovery (MAX_PATH/PATH_MAX/etc) in release builds
TODO: Move to Core module when such a thing exists 

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
----------------------------------------------pal.cpp-----------------------------------------

TODO: Move to Core module

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
----------------------------------------------umath.h-----------------------------------------

TODO: Change library to always return an instance, never operate on a reference
TODO: Quaternions

----------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////
---------------------------------------------rendering----------------------------------------

TODO: Fix issues with rotating objects by removing sliders and instead performing rotations
      directly on the existing matrix. We don't need to track the current rotation, it all
      can just be changes to the current state. Will need to implement error accumulation
      mitigation though.

TODO: Basic lighting and shadows for multiple lights/types
TODO: PBR implementation
TODO: Holographic radiance cascades

----------------------------------------------------------------------------------------------

*/
