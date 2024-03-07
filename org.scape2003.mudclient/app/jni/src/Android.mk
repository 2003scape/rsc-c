LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_IMAGE_PATH := ../SDL_image
CGLM_PATH := ../cglm

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
					$(LOCAL_PATH)/$(SDL_IMAGE_PATH)/include \
					$(LOCAL_PATH)/$(CGLM_PATH)/include

# Add your application source files here...
# glob didn't work :(
# RENDER_SW
#LOCAL_SRC_FILES := src/chat-message.c src/custom/clarify-herblaw-items.c src/custom/diverse-npcs.c src/custom/item-highlight.c src/game-character.c src/game-data.c src/game-model.c src/lib/bn.c src/lib/bzip.c src/lib/ini.c src/lib/isaac.c src/mudclient.c src/options.c src/packet-handler.c src/packet-stream.c src/panel.c src/polygon.c src/scene.c src/surface.c src/ui/additional-options.c src/ui/appearance.c src/ui/bank.c src/ui/combat-style.c src/ui/confirm.c src/ui/duel.c src/ui/experience-drops.c src/ui/inventory-tab.c src/ui/login.c src/ui/logout.c src/ui/lost-connection.c src/ui/magic-tab.c src/ui/menu.c src/ui/message-tabs.c src/ui/minimap-tab.c src/ui/offer-x.c src/ui/option-menu.c src/ui/options-tab.c src/ui/server-message.c src/ui/shop.c src/ui/sleep.c src/ui/social-tab.c src/ui/stats-tab.c src/ui/status-bars.c src/ui/trade.c src/ui/transaction.c src/ui/ui-tabs.c src/ui/welcome.c src/ui/wilderness-warning.c src/utility.c src/world.c

# RENDER_GL
LOCAL_SRC_FILES = src/chat-message.c src/custom/clarify-herblaw-items.c src/custom/diverse-npcs.c src/custom/item-highlight.c src/game-character.c src/game-data.c src/game-model.c src/lib/bn.c src/lib/bzip.c src/lib/ini.c src/lib/isaac.c src/mudclient.c src/options.c src/packet-handler.c src/packet-stream.c src/panel.c src/polygon.c src/scene.c src/surface.c src/ui/additional-options.c src/ui/appearance.c src/ui/bank.c src/ui/combat-style.c src/ui/confirm.c src/ui/duel.c src/ui/experience-drops.c src/ui/inventory-tab.c src/ui/login.c src/ui/logout.c src/ui/lost-connection.c src/ui/magic-tab.c src/ui/menu.c src/ui/message-tabs.c src/ui/minimap-tab.c src/ui/offer-x.c src/ui/option-menu.c src/ui/options-tab.c src/ui/server-message.c src/ui/shop.c src/ui/sleep.c src/ui/social-tab.c src/ui/stats-tab.c src/ui/status-bars.c src/ui/trade.c src/ui/transaction.c src/ui/ui-tabs.c src/ui/welcome.c src/ui/wilderness-warning.c src/utility.c src/world.c src/gl/shader.c src/gl/vertex-buffer.c src/gl/textures/fonts.c src/gl/textures/media.c src/gl/textures/model_textures.c src/gl/textures/entities.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
