LOCAL_PATH := $(call my-dir)/../source


include $(CLEAR_VARS)



# Uncomment for the correct headset - slight changes required in OpenXR implementation
OPENXR_HMD = -DMETA_QUEST
#OPENXR_HMD = -DPICO_XR



LOCAL_SHORT_COMMANDS := true
APP_SHORT_COMMANDS := true

LOCAL_MODULE    := raze

LOCAL_CFLAGS   := $(OPENXR_HMD) -funsigned-char  -DHAVE_GLES2 -DUSE_OPENGL -DNO_CLOCK_GETTIME -DUSE_GL_HW_BUFFERS -fvisibility=hidden -frtti  -D__MOBILE__  -DOPNMIDI_DISABLE_GX_EMULATOR -DGZDOOM  -DGZDOOM_GL3 -D__STDINT_LIMITS -DENGINE_NAME=\"gzdoom_dev\"
#-DNO_PIX_BUFF
#-DUSE_GL_HW_BUFFERS
#-DHAVE_VULKAN
#-DUSE_GL_HW_BUFFERS

LOCAL_CPPFLAGS := $(OPENXR_HMD) -include g_pch.h -DHAVE_FLUIDSYNTH -DHAVE_MPG123 -DHAVE_SNDFILE -std=c++17  -Wno-inconsistent-missing-override -Werror=format-security  -fexceptions -fpermissive -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -D__forceinline=inline -DNO_GTK -DNO_SSE

LOCAL_CFLAGS  += -DNO_SEND_STATS

LOCAL_CFLAGS  += -DOPNMIDI_USE_LEGACY_EMULATOR
LOCAL_CFLAGS  += -DADLMIDI_DISABLE_MUS_SUPPORT -DADLMIDI_DISABLE_XMI_SUPPORT -DADLMIDI_DISABLE_MIDI_SEQUENCER
LOCAL_CFLAGS  += -DOPNMIDI_DISABLE_MUS_SUPPORT -DOPNMIDI_DISABLE_XMI_SUPPORT -DOPNMIDI_DISABLE_MIDI_SEQUENCER

ifeq ($(BUILD_SERIAL),1)
LOCAL_CPPFLAGS += -DANTI_HACK 
endif

	
LOCAL_C_INCLUDES := \
	$(TOP_DIR)/../../../../../3rdParty/khronos/openxr/OpenXR-SDK/include \
	$(TOP_DIR)/../../../../../3rdParty/khronos/openxr/OpenXR-SDK/src/common \
    $(TOP_DIR)/../../../../../OpenXR/Include \
    $(TOP_DIR)/ \  \
    $(SUPPORT_LIBS)/fluidsynth-lite/include \
    $(GZDOOM_TOP_PATH)/source/ \
	$(GZDOOM_TOP_PATH)/source/core \
	$(GZDOOM_TOP_PATH)/source/core/2d \
	$(GZDOOM_TOP_PATH)/source/core/fonts \
	$(GZDOOM_TOP_PATH)/source/core/utility \
	$(GZDOOM_TOP_PATH)/source/core/console \
	$(GZDOOM_TOP_PATH)/source/core/textures \
	$(GZDOOM_TOP_PATH)/source/core/music \
	$(GZDOOM_TOP_PATH)/source/core/dobject \
	$(GZDOOM_TOP_PATH)/source/core/menu \
	$(GZDOOM_TOP_PATH)/source/core/input \
	$(GZDOOM_TOP_PATH)/source/platform \
	$(GZDOOM_TOP_PATH)/source/common/cutscenes \
	$(GZDOOM_TOP_PATH)/source/common/audio/sound \
	$(GZDOOM_TOP_PATH)/source/common/audio/music \
	$(GZDOOM_TOP_PATH)/source/common/2d \
	$(GZDOOM_TOP_PATH)/source/common/thirdparty \
	$(GZDOOM_TOP_PATH)/source/common/textures \
	$(GZDOOM_TOP_PATH)/source/common/textures/formats \
	$(GZDOOM_TOP_PATH)/source/common/textures/hires \
	$(GZDOOM_TOP_PATH)/source/common/textures \
	$(GZDOOM_TOP_PATH)/source/common/models \
	$(GZDOOM_TOP_PATH)/source/common/filesystem \
	$(GZDOOM_TOP_PATH)/source/common/utility \
	$(GZDOOM_TOP_PATH)/source/common/console \
	$(GZDOOM_TOP_PATH)/source/common/engine \
	$(GZDOOM_TOP_PATH)/source/common/menu \
	$(GZDOOM_TOP_PATH)/source/common/statusbar \
	$(GZDOOM_TOP_PATH)/source/common/fonts \
	$(GZDOOM_TOP_PATH)/source/common/objects \
	$(GZDOOM_TOP_PATH)/source/common/startscreen \
	$(GZDOOM_TOP_PATH)/source/common/rendering \
	$(GZDOOM_TOP_PATH)/source/common/rendering/hwrenderer \
	$(GZDOOM_TOP_PATH)/source/common/rendering/hwrenderer/data \
	$(GZDOOM_TOP_PATH)/source/common/rendering/gl_load \
	$(GZDOOM_TOP_PATH)/source/common/rendering/gl \
    $(GZDOOM_TOP_PATH)/source/common/rendering/gles \
	$(GZDOOM_TOP_PATH)/source/common/rendering/vulkan/thirdparty \
	$(GZDOOM_TOP_PATH)/source/common/rendering/polyrenderer/backend \
	$(GZDOOM_TOP_PATH)/source/common/rendering/polyrenderer/drawers \
	$(GZDOOM_TOP_PATH)/source/common/scripting/vm \
	$(GZDOOM_TOP_PATH)/source/common/scripting/jit \
	$(GZDOOM_TOP_PATH)/source/common/scripting/core \
	$(GZDOOM_TOP_PATH)/source/common/scripting/interface \
	$(GZDOOM_TOP_PATH)/source/common/scripting/frontend \
	$(GZDOOM_TOP_PATH)/source/common/scripting/backend \
	$(GZDOOM_TOP_PATH)/source/common/platform/posix \
	$(GZDOOM_TOP_PATH)/source/common/platform/posix/nosdl \
	$(GZDOOM_TOP_PATH)/source/core/rendering \
	$(GZDOOM_TOP_PATH)/source/core/rendering/scene \
	$(GZDOOM_TOP_PATH)/source/common/thirdparty/libsmackerdec/include \
	$(GZDOOM_TOP_PATH)/source/thirdparty/include \
	$(GZDOOM_TOP_PATH)/source/build/include \
    $(GZDOOM_TOP_PATH)/libraries/lzma/C \
    $(GZDOOM_TOP_PATH)/libraries/bzip2 \
    $(GZDOOM_TOP_PATH)/libraries/gdtoa \
    $(GZDOOM_TOP_PATH)/libraries/libtess/Include \
    $(GZDOOM_TOP_PATH)/libraries/discordrpc/include \
    $(GZDOOM_TOP_PATH)/android/src/vpx/include \
    $(GZDOOM_TOP_PATH)/source/common/rendering/gles/glad/include \
	$(SUPPORT_LIBS)/openal/include/AL \
	$(SUPPORT_LIBS)/jpeg8d \
	$(SUPPORT_LIBS) \
	$(SUPPORT_LIBS)/ZMusic/include \
	$(GZDOOM_TOP_PATH)/android/src/extrafiles  \
	$(GZDOOM_TOP_PATH)/android/src


#############################################################################
# CLIENT/SERVER
#############################################################################


ANDROID_SRC_FILES = \
     ../android/src/i_specialpaths_android.cpp


PLAT_POSIX_SOURCES = \
	platform/posix/i_steam.cpp \
    common/platform/posix/i_system_posix.cpp \


PLAT_SDL_SOURCES = \
		common/platform/posix/nosdl/crashcatcher.c \
    	common/platform/posix/nosdl/hardware.cpp \
    	common/platform/posix/nosdl/i_gui.cpp \
    	common/platform/posix/nosdl/i_main.cpp \
    	common/platform/posix/nosdl/i_system.cpp \
    	common/platform/posix/nosdl/i_input.cpp \
    	common/platform/posix/nosdl/i_joystick.cpp \
    	common/platform/posix/nosdl/glvideo.cpp \
    	common/platform/posix/nosdl/st_start.cpp \


 FASTMATH_SOURCES = \
	../android/src/android_gl_load.c \
	common/textures/hires/hqnx/init.cpp \
	common/textures/hires/hqnx/hq2x.cpp \
	common/textures/hires/hqnx/hq3x.cpp \
	common/textures/hires/hqnx/hq4x.cpp \
	common/textures/hires/xbr/xbrz.cpp \
	common/textures/hires/xbr/xbrz_old.cpp \
	common/utility/matrix.cpp



PCH_SOURCES = \
	common/thirdparty/richpresence.cpp \
	thirdparty/src/md4.cpp \
	build/src/clip.cpp \
	core/actorinfo.cpp \
	core/zcc_compile_raze.cpp \
	core/vmexports.cpp \
	core/thingdef_data.cpp \
	core/thingdef_properties.cpp \
	core/actorlist.cpp \
	core/automap.cpp \
	core/cheats.cpp \
	core/cheathandler.cpp \
	core/rts.cpp \
	core/ct_chat.cpp \
	core/d_net.cpp \
	core/d_protocol.cpp \
	core/mainloop.cpp \
	core/gameconfigfile.cpp \
	core/gamecvars.cpp \
	core/gamecontrol.cpp \
	core/gamehud.cpp \
	core/gamefuncs.cpp \
	core/gameinput.cpp \
	core/g_mapinfo.cpp \
	core/interpolate.cpp \
	core/inputstate.cpp \
	core/maphack.cpp \
	core/mapinfo.cpp \
	core/maploader.cpp \
	core/postprocessor.cpp \
	core/searchpaths.cpp \
	core/screenjob.cpp \
	core/initfs.cpp \
	core/statistics.cpp \
	core/secrets.cpp \
	core/savegamehelp.cpp \
	core/precache.cpp \
	core/psky.cpp \
	core/quotes.cpp \
	core/screenshot.cpp \
	core/sectorgeometry.cpp \
	core/razefont.cpp \
	core/raze_music.cpp \
	core/raze_sound.cpp \
	core/palette.cpp \
	core/zcompile.cpp \
	core/statusbar2.cpp \
	core/shared_hud.cpp \
	core/gi.cpp \
	core/defparser.cpp \
	core/rendering/hw_entrypoint.cpp \
	core/rendering/hw_models.cpp \
	core/rendering/hw_voxels.cpp \
	core/rendering/hw_palmanager.cpp \
	core/rendering/hw_sections.cpp \
	core/rendering/hw_vertexmap.cpp \
	core/rendering/scene/hw_clipper.cpp \
	core/rendering/scene/hw_walls.cpp \
	core/rendering/scene/hw_walls_vertex.cpp \
	core/rendering/scene/hw_flats.cpp \
	core/rendering/scene/hw_sprites.cpp \
	core/rendering/scene/hw_drawlistadd.cpp \
	core/rendering/scene/hw_drawlist.cpp \
	core/rendering/scene/hw_drawinfo.cpp \
	core/rendering/scene/hw_bunchdrawer.cpp \
	core/rendering/scene/hw_portal.cpp \
	core/rendering/scene/hw_skyportal.cpp \
	core/rendering/scene/hw_sky.cpp \
	core/rendering/scene/hw_setcolor.cpp \
	core/rendering/scene/hw_lighting.cpp \
	core/r_data/gldefs.cpp \
	core/models/modeldata.cpp \
	core/console/c_notifybuffer.cpp \
	core/console/d_event.cpp \
	common/audio/sound/i_sound.cpp \
	common/audio/sound/oalsound.cpp \
	common/audio/sound/s_environment.cpp \
	common/audio/sound/s_sound.cpp \
	common/audio/sound/s_reverbedit.cpp \
	common/audio/music/music_midi_base.cpp \
	common/audio/music/music.cpp \
	common/audio/music/i_music.cpp \
	common/audio/music/i_soundfont.cpp \
	common/audio/music/music_config.cpp \
	common/2d/v_2ddrawer.cpp \
	common/2d/v_drawtext.cpp \
	common/2d/v_draw.cpp \
	common/2d/wipe.cpp \
	common/thirdparty/gain_analysis.cpp \
	common/thirdparty/sfmt/SFMT.cpp \
	common/startscreen/startscreen.cpp \
	common/startscreen/startscreen_heretic.cpp \
	common/startscreen/startscreen_hexen.cpp \
	common/startscreen/startscreen_strife.cpp \
	common/startscreen/startscreen_generic.cpp \
	common/startscreen/endoom.cpp \
	common/fonts/singlelumpfont.cpp \
	common/fonts/singlepicfont.cpp \
	common/fonts/specialfont.cpp \
	common/fonts/font.cpp \
	common/fonts/hexfont.cpp \
	common/fonts/v_font.cpp \
	common/fonts/v_text.cpp	 \
	common/textures/hw_ihwtexture.cpp \
	common/textures/hw_material.cpp \
	common/textures/bitmap.cpp \
	common/textures/m_png.cpp \
	common/textures/texture.cpp \
	common/textures/gametexture.cpp \
	common/textures/image.cpp \
	common/textures/imagetexture.cpp \
	common/textures/texturemanager.cpp \
	common/textures/multipatchtexturebuilder.cpp \
	common/textures/skyboxtexture.cpp \
	common/textures/animtexture.cpp \
	common/textures/v_collection.cpp \
	common/textures/animlib.cpp \
	common/textures/formats/automaptexture.cpp \
	common/textures/formats/brightmaptexture.cpp \
	common/textures/formats/buildtexture.cpp \
	common/textures/formats/ddstexture.cpp \
	common/textures/formats/flattexture.cpp \
	common/textures/formats/fontchars.cpp \
	common/textures/formats/imgztexture.cpp \
	common/textures/formats/jpegtexture.cpp \
	common/textures/formats/md5check.cpp \
	common/textures/formats/multipatchtexture.cpp \
	common/textures/formats/patchtexture.cpp \
	common/textures/formats/pcxtexture.cpp \
	common/textures/formats/pngtexture.cpp \
	common/textures/formats/rawpagetexture.cpp \
	common/textures/formats/startuptexture.cpp \
	common/textures/formats/emptytexture.cpp \
	common/textures/formats/shadertexture.cpp \
	common/textures/formats/tgatexture.cpp \
	common/textures/formats/stbtexture.cpp \
	common/textures/formats/anmtexture.cpp \
	common/textures/formats/startscreentexture.cpp \
	common/textures/hires/hqresize.cpp \
	common/models/models_md3.cpp \
	common/models/models_md2.cpp \
	common/models/models_voxel.cpp \
	common/models/models_ue1.cpp \
	common/models/models_obj.cpp \
	common/models/models_iqm.cpp \
	common/models/model.cpp \
	common/models/voxels.cpp \
	common/console/c_commandline.cpp \
	common/console/c_buttons.cpp \
	common/console/c_bind.cpp \
	common/console/c_enginecmds.cpp \
	common/console/c_consolebuffer.cpp \
	common/console/c_cvars.cpp \
	common/console/c_dispatch.cpp \
	common/console/c_commandbuffer.cpp \
	common/console/c_console.cpp \
	common/console/c_notifybufferbase.cpp \
	common/console/c_tabcomplete.cpp \
	common/console/c_expr.cpp \
	common/cutscenes/playmve.cpp \
	common/cutscenes/movieplayer.cpp \
	common/cutscenes/screenjob.cpp \
	common/utility/engineerrors.cpp \
	common/utility/i_module.cpp \
	common/utility/gitinfo.cpp \
	common/utility/m_alloc.cpp \
	common/utility/utf8.cpp \
	common/utility/palette.cpp \
	common/utility/files.cpp \
	common/utility/files_decompress.cpp \
	common/utility/memarena.cpp \
	common/utility/cmdlib.cpp \
	common/utility/configfile.cpp \
	common/utility/i_time.cpp \
	common/utility/m_argv.cpp \
	common/utility/s_playlist.cpp \
	common/utility/zstrformat.cpp \
	common/utility/name.cpp \
	common/utility/r_memory.cpp \
	common/thirdparty/base64.cpp \
	common/thirdparty/md5.cpp \
 	common/thirdparty/superfasthash.cpp \
	common/thirdparty/libsmackerdec/src/BitReader.cpp \
	common/thirdparty/libsmackerdec/src/FileStream.cpp \
	common/thirdparty/libsmackerdec/src/HuffmanVLC.cpp \
	common/thirdparty/libsmackerdec/src/LogError.cpp \
	common/thirdparty/libsmackerdec/src/SmackerDecoder.cpp \
	common/filesystem/filesystem.cpp \
	common/filesystem/ancientzip.cpp \
	common/filesystem/file_7z.cpp \
	common/filesystem/file_grp.cpp \
	common/filesystem/file_lump.cpp \
	common/filesystem/file_rff.cpp \
	common/filesystem/file_wad.cpp \
	common/filesystem/file_zip.cpp \
	common/filesystem/file_pak.cpp \
	common/filesystem/file_whres.cpp \
	common/filesystem/file_ssi.cpp \
	common/filesystem/file_directory.cpp \
	common/filesystem/resourcefile.cpp \
	common/engine/cycler.cpp \
	common/engine/d_event.cpp \
	common/engine/date.cpp \
	common/engine/stats.cpp \
	common/engine/sc_man.cpp \
	common/engine/palettecontainer.cpp \
	common/engine/stringtable.cpp \
	common/engine/i_net.cpp \
	common/engine/i_interface.cpp \
	common/engine/renderstyle.cpp \
	common/engine/v_colortables.cpp \
	common/engine/serializer.cpp \
	common/engine/m_joy.cpp \
	common/engine/m_random.cpp \
	common/objects/autosegs.cpp \
	common/objects/dobject.cpp \
	common/objects/dobjgc.cpp \
	common/objects/dobjtype.cpp \
	common/menu/joystickmenu.cpp \
	common/menu/menu.cpp \
	common/menu/messagebox.cpp \
	common/menu/optionmenu.cpp \
	common/menu/resolutionmenu.cpp \
	common/menu/menudef.cpp \
	common/menu/savegamemanager.cpp \
	common/statusbar/base_sbar.cpp	 \
	common/rendering/v_framebuffer.cpp \
	common/rendering/v_video.cpp \
	common/rendering/r_thread.cpp \
	common/rendering/r_videoscale.cpp \
	common/rendering/hwrenderer/hw_draw2d.cpp \
	common/rendering/hwrenderer/data/hw_clock.cpp \
	common/rendering/hwrenderer/data/hw_skydome.cpp \
	common/rendering/hwrenderer/data/flatvertices.cpp \
	common/rendering/hwrenderer/data/hw_viewpointbuffer.cpp \
	common/rendering/hwrenderer/data/hw_modelvertexbuffer.cpp \
	common/rendering/hwrenderer/data/hw_cvars.cpp \
	common/rendering/hwrenderer/data/hw_vrmodes.cpp \
	common/rendering/hwrenderer/data/hw_lightbuffer.cpp \
	common/rendering/hwrenderer/data/hw_bonebuffer.cpp \
	common/rendering/hwrenderer/data/hw_aabbtree.cpp \
	common/rendering/hwrenderer/data/hw_shadowmap.cpp \
	common/rendering/hwrenderer/data/hw_shaderpatcher.cpp \
	common/rendering/hwrenderer/postprocessing/hw_postprocessshader.cpp \
	common/rendering/hwrenderer/postprocessing/hw_postprocess.cpp \
	common/rendering/hwrenderer/postprocessing/hw_postprocess_cvars.cpp \
	common/rendering/hwrenderer/postprocessing/hw_postprocessshader_ccmds.cpp \
	common/rendering/gl_load/gl_interface.cpp \
	common/rendering/gl/gl_renderer.cpp \
	common/rendering/gl/gl_stereo3d.cpp \
	common/rendering/gl/gl_framebuffer.cpp \
	common/rendering/gl/gl_renderstate.cpp \
	common/rendering/gl/gl_renderbuffers.cpp \
	common/rendering/gl/gl_postprocess.cpp \
	common/rendering/gl/gl_postprocessstate.cpp \
	common/rendering/gl/gl_debug.cpp \
	common/rendering/gl/gl_buffers.cpp \
	common/rendering/gl/gl_hwtexture.cpp \
	common/rendering/gl/gl_samplers.cpp \
	common/rendering/gl/gl_shader.cpp \
	common/rendering/gl/gl_shaderprogram.cpp \
	common/scripting/core/dictionary.cpp \
	common/scripting/core/dynarrays.cpp \
	common/scripting/core/symbols.cpp \
	common/scripting/core/types.cpp \
	common/scripting/core/scopebarrier.cpp \
	common/scripting/core/vmdisasm.cpp \
	common/scripting/core/imports.cpp \
	common/scripting/vm/vmexec.cpp \
	common/scripting/vm/vmframe.cpp \
	common/scripting/interface/stringformat.cpp \
	common/scripting/interface/vmnatives.cpp \
	common/scripting/frontend/ast.cpp \
	common/scripting/frontend/zcc_compile.cpp \
	common/scripting/frontend/zcc_parser.cpp \
	common/scripting/backend/vmbuilder.cpp \
	common/scripting/backend/codegen.cpp \
	core/textures/tiletexture.cpp \
	core/textures/texinfo.cpp \
	core/textures/tilesetbuilder.cpp \
	core/textures/skytexture.cpp \
	core/textures/hightile.cpp \
	core/music/s_advsound.cpp \
	core/menu/loadsavemenu.cpp \
	core/menu/usermap.cpp \
	core/menu/razemenu.cpp \
	games/duke/all.cpp \
	games/duke/all_d.cpp \
	games/duke/all_r.cpp \
	games/exhumed/all.cpp \
	games/blood/all.cpp \
	games/sw/all.cpp

RAZEXR_SRC_FILES :=  ${TOP_DIR}/RazeXR/RazeXR_OpenXR.cpp \
       ${TOP_DIR}/RazeXR/TBXR_Common.cpp \
       ${TOP_DIR}/RazeXR/VrInputCommon.cpp \
       ${TOP_DIR}/RazeXR/VrInputDefault.cpp \
       ${TOP_DIR}/RazeXR/mathlib.c \
       ${TOP_DIR}/RazeXR/matrixlib.c \
       ${TOP_DIR}/RazeXR/argtable3.c \
       ${TOP_DIR}/RazeXR/OpenXRInput_MetaQuest.cpp \
       ${TOP_DIR}/RazeXR/OpenXRInput_PicoXR.cpp

SYSTEM_SOURCES  = ${PLAT_POSIX_SOURCES} ${PLAT_SDL_SOURCES} ${PLAT_UNIX_SOURCES}

#$(ANDROID_SRC_FILES) \

LOCAL_SRC_FILES = \
    ${ANDROID_SRC_FILES} \
    ${RAZEXR_SRC_FILES} \
	${SYSTEM_SOURCES} \
	${FASTMATH_SOURCES} \
	${PCH_SOURCES} \
	common/utility/x86.cpp \
	common/thirdparty/strnatcmp.c \
	common/utility/zstring.cpp \
	common/utility/findfile.cpp \
	common/thirdparty/math/asin.c \
	common/thirdparty/math/atan.c \
	common/thirdparty/math/const.c \
	common/thirdparty/math/cosh.c \
	common/thirdparty/math/exp.c \
	common/thirdparty/math/isnan.c \
	common/thirdparty/math/log.c \
	common/thirdparty/math/log10.c \
	common/thirdparty/math/mtherr.c \
	common/thirdparty/math/polevl.c \
	common/thirdparty/math/pow.c \
	common/thirdparty/math/powi.c \
	common/thirdparty/math/sin.c \
	common/thirdparty/math/sinh.c \
	common/thirdparty/math/sqrt.c \
	common/thirdparty/math/tan.c \
	common/thirdparty/math/tanh.c \
	common/thirdparty/math/fastsin.cpp \



LOCAL_LDLIBS := -ldl -llog -lOpenSLES -landroid
LOCAL_LDLIBS +=  -lEGL -lGLESv3

LOCAL_STATIC_LIBRARIES :=  zlib_gl3 lzma_gl3 gdtoa_gl3  bzip2_gl3 jpeg vpx tess_gl3
LOCAL_SHARED_LIBRARIES :=  openxr_loader openal zmusic

include $(BUILD_SHARED_LIBRARY)


$(call import-module,OpenXR/Projects/AndroidPrebuilt/jni)

