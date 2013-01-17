the code is chang from ogrekit project,

have two problem need to solve

1\the fonts messup when use ogre gles1 render,   https://github.com/yaxinhoo/ogrelibrocketkit/issues/1
2\nothing render  when use ogre gles2 on iphone. 

you can cmake the code on osx  , then check to open the OGREKIT_BUILD_IPHONE and OGREKIT_COMPILE_LIBROCKET , then open the OGREKIT_BUILD_GLESRS/OGREKIT_BUILD_GLES2RS to use gles1 or gles 2.  