cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(PROJECT_NAME Tower_defense)

#set(EDITOR_BUILD)

set(ENGINE_PUBLIC_INCLUDES_EDITOR	
	includes/EngineCore/Editor/EditorApplication.h	

	includes/EngineCore/Editor/UI/UImodule.h
	includes/EngineCore/Editor/UI/UIlayouts.h		   
)

set(ENGINE_PUBLIC_SRC_EDITOR			
	src/Editor/EditorApplication.cpp	
	
	src/Editor/UI/UImodule.cpp	
	src/Editor/UI/UIlayouts.cpp
)

set(ENGINE_PUBLIC_INCLUDES
	includes/EngineCore/Engine/Engine.h
	includes/EngineCore/Engine/Application.h

	includes/EngineCore/IComponent.h
	includes/EngineCore/IGameObject.h
	includes/EngineCore/IGame.h
	includes/EngineCore/System/Event.h
	includes/EngineCore/System/Keys.h
	includes/EngineCore/System/Input.h
		   
	includes/EngineCore/System/LogSystem.h
	includes/EngineCore/System/Log.h
	includes/EngineCore/System/Timer.h
	includes/EngineCore/System/TemplateTimer.h
	includes/EngineCore/System/Stopwatch.h
	includes/EngineCore/System/List.h	
	includes/EngineCore/System/SysFunc.h	
	includes/EngineCore/System/CommandManager.h	
	includes/EngineCore/System/INI_loader.h	
	includes/EngineCore/System/AppInfo.h
	includes/EngineCore/System/ImageLoader.h			
	  
	includes/EngineCore/GUI/GUI_place.h
	includes/EngineCore/GUI/GUI_element.h
	includes/EngineCore/GUI/Button.h
	includes/EngineCore/GUI/Sprite.h
	includes/EngineCore/GUI/Square.h
	includes/EngineCore/GUI/Font.h
	includes/EngineCore/GUI/TextRenderer.h
	includes/EngineCore/GUI/InputField.h
	includes/EngineCore/GUI/CheckBox.h
	includes/EngineCore/GUI/ChatBox.h
	includes/EngineCore/GUI/ScrollBox.h
	includes/EngineCore/GUI/Slider.h
	includes/EngineCore/GUI/BindButton.h
	includes/EngineCore/GUI/Table.h

	includes/Games/Tower_Defense/GameApplication.h  
	includes/Games/Tower_Defense/Castle.h
	includes/Games/Tower_Defense/HealthBar.h
	includes/Games/Tower_Defense/BaseEnemy.h
	includes/Games/Tower_Defense/BaseTower.h
	includes/Games/Tower_Defense/BaseEffect.h
	includes/Games/Tower_Defense/TowerMortar.h
	includes/Games/Tower_Defense/TowerArcher.h
	includes/Games/Tower_Defense/TowerIce.h
	includes/Games/Tower_Defense/TowerExecutioner.h
	includes/Games/Tower_Defense/TowerInferno.h
	includes/Games/Tower_Defense/EnemyMonkey.h
	includes/Games/Tower_Defense/EnemyMagician.h
	includes/Games/Tower_Defense/EnemyBug.h
	includes/Games/Tower_Defense/EnemyRobot.h
	includes/Games/Tower_Defense/EnemyProfessor.h
	includes/Games/Tower_Defense/Target.h
	includes/Games/Tower_Defense/DamageTable.h
	includes/Games/Tower_Defense/FreezeEffect.h
	includes/Games/Tower_Defense/HealthEffect.h
)

set(ENGINE_PRIVATE_INCLUDES
	includes/EngineCore/Engine/Window.h
	
	includes/EngineCore/Resources/ResourceManager.h
	includes/EngineCore/Resources/stb_image.h
	includes/EngineCore/Resources/Scene.h
	includes/EngineCore/Resources/RegistryManager.h
	
	includes/EngineCore/Renderer/ShaderProgram.h
	includes/EngineCore/Renderer/Texture2D.h
	includes/EngineCore/Renderer/VertexBuffer.h
	includes/EngineCore/Renderer/IndexBuffer.h
	includes/EngineCore/Renderer/Renderer.h
	includes/EngineCore/Renderer/VertexArray.h
	includes/EngineCore/Renderer/VertexBufferLayout.h
	includes/EngineCore/Renderer/Point.h
	includes/EngineCore/Renderer/Line.h
	includes/EngineCore/Renderer/Camera.h
	includes/EngineCore/Renderer/Material.h
	includes/EngineCore/Renderer/ShaderProgramLayout.h
	includes/EngineCore/Renderer/Circle.h
	
	includes/EngineCore/Physics/PhysicsEngine.h
	includes/EngineCore/Physics/Collider.h
	includes/EngineCore/Physics/MoveController.h		
	includes/EngineCore/Physics/Ray.h	
		
	includes/EngineCore/Sound/SoundEngine.h	
	includes/EngineCore/Sound/Sound.h	
	includes/EngineCore/Sound/miniaudio.h
	
	includes/EngineCore/Renderer3D/GraphicsObject.h
	
	includes/EngineCore/System/ShadersSettings.h
	
	includes/EngineCore/Components/MeshRenderer.h	
	includes/EngineCore/Components/Transform.h
	includes/EngineCore/Components/SpriteRenderer.h
	includes/EngineCore/Components/Highlight.h
	
	includes/EngineCore/Meshes/Cube.h
	includes/EngineCore/Meshes/ObjModel.h
	includes/EngineCore/Meshes/EmptyObject.h
	includes/EngineCore/Meshes/Plane.h	
	includes/EngineCore/Meshes/Grid.h	
  
	includes/EngineCore/Light/DirectionalLight.h
	includes/EngineCore/Light/PointerLight.h

	includes/EngineCore/Network/WinSock.h
)

set(ENGINE_PRIVATE_SOURCES
	src/Engine/Engine.cpp
	src/Engine/Window.cpp
	src/Engine/Application.cpp
	src/Engine/Input.cpp
	
	#src/Examples/TanksDemo/TanksDemoGame.cpp
	#src/Examples/TanksDemo/TanksDemoGame.h
	#src/Examples/TanksDemo/Tank.cpp
	#src/Examples/TanksDemo/Tank.h
	#src/Examples/TanksDemo/BrickWall.h
	#src/Examples/TanksDemo/BrickWall.cpp
	#src/Examples/TanksDemo/Bullet.h
	#src/Examples/TanksDemo/Bullet.cpp	
	#src/Examples/SnakeDemo/SnakeDemoGame.cpp
	#src/Examples/SnakeDemo/SnakeDemoGame.h
	#src/Examples/SnakeDemo/SnakePart.h
	#src/Examples/SnakeDemo/SnakePart.cpp
	#src/Examples/SnakeDemo/SnakePart.h	
	#src/Examples/PongDemo/PongDemoGame.h
	#src/Examples/PongDemo/PongDemoGame.cpp
	#src/Examples/PongDemo/PongObject.h
	#src/Examples/PongDemo/PongObject.cpp
	#src/Examples/PongDemo/Ball.h
	#src/Examples/PongDemo/Ball.cpp
	
	src/Examples/Tower_Defense/GameApplication.cpp
	src/Examples/Tower_Defense/Castle.cpp
	src/Examples/Tower_Defense/HealthBar.cpp
	src/Examples/Tower_Defense/BaseEnemy.cpp
	src/Examples/Tower_Defense/BaseTower.cpp
	src/Examples/Tower_Defense/BaseEffect.cpp
	src/Examples/Tower_Defense/TowerMortar.cpp
	src/Examples/Tower_Defense/TowerArcher.cpp
	src/Examples/Tower_Defense/TowerExecutioner.cpp
	src/Examples/Tower_Defense/TowerInferno.cpp
	src/Examples/Tower_Defense/TowerIce.cpp
	src/Examples/Tower_Defense/EnemyRobot.cpp
	src/Examples/Tower_Defense/EnemyMonkey.cpp
	src/Examples/Tower_Defense/EnemyBug.cpp
	src/Examples/Tower_Defense/EnemyMagician.cpp
	src/Examples/Tower_Defense/EnemyProfessor.cpp
	src/Examples/Tower_Defense/FreezeEffect.cpp
	src/Examples/Tower_Defense/HealthEffect.cpp
	
	src/Resources/ResourceManager.cpp
	src/Resources/Scene.cpp
	src/Resources/RegistryManager.cpp	
	
	src/Renderer/ShaderProgram.cpp
	src/Renderer/Texture2D.cpp
	src/Renderer/VertexBuffer.cpp
	src/Renderer/IndexBuffer.cpp
	src/Renderer/Renderer.cpp
	src/Renderer/VertexArray.cpp
	src/Renderer/VertexBufferLayout.cpp
	src/Renderer/Point.cpp
	src/Renderer/Line.cpp
	src/Renderer/Camera.cpp	
	src/Renderer/Material.cpp
	src/Renderer/ShaderProgramLayout.cpp
	src/Renderer/Circle.cpp
	
	src/Physics/PhysicsEngine.cpp
	src/Physics/Collider.cpp
	src/Physics/MoveController.cpp
	src/Physics/Ray.cpp

	src/Sound/SoundEngine.cpp
	src/Sound/Sound.cpp	
	src/Sound/miniaudio.cpp
	
	src/System/Timer.cpp
	src/System/Stopwatch.cpp	
	src/System/LogSystem.cpp	
	src/System/SysFunc.cpp	
	src/System/SysFunc.cpp	
	src/System/CommandManager.cpp
	src/System/INI_loader.cpp		
	src/System/ImageLoader.cpp			
	
	src/Components/MeshRenderer.cpp	
	src/Components/Transform.cpp
	src/Components/SpriteRenderer.cpp
	src/Components/Highlight.cpp
	
	src/Meshes/Cube.cpp
	src/Meshes/ObjModel.cpp
	src/Meshes/Plane.cpp
	src/Meshes/Grid.cpp
		
	src/Light/DirectionalLight.cpp
	src/Light/PointerLight.cpp
	
	src/GUI/GUI_place.cpp
	src/GUI/Button.cpp
	src/GUI/Sprite.cpp
	src/GUI/Square.cpp
	src/GUI/Font.cpp
	src/GUI/TextRenderer.cpp
	src/GUI/Button.cpp
	src/GUI/InputField.cpp
	src/GUI/CheckBox.cpp
	src/GUI/ChatBox.cpp
	src/GUI/ScrollBox.cpp
	src/GUI/Slider.cpp
	src/GUI/BindButton.cpp
	src/GUI/Table.cpp

	src/Network/WinSock.cpp
)


project(${PROJECT_NAME})


	add_executable(${PROJECT_NAME}
		src/main.cpp
		src/appinfo.rc
		${ENGINE_PRIVATE_SOURCES}
		${ENGINE_PUBLIC_INCLUDES}
		${ENGINE_PRIVATE_INCLUDES}	
#if (DEFINED EDITOR_BUILD)				
#		${ENGINE_PUBLIC_INCLUDES_EDITOR}
#		${ENGINE_PUBLIC_SRC_EDITOR}
#endif()
	)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)
add_subdirectory(external/glad)
add_subdirectory(external/freetype)

target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glad)
target_link_libraries(${PROJECT_NAME} freetype)

target_include_directories(${PROJECT_NAME} PRIVATE includes)
include_directories(external/glm)
include_directories(external/rapidjson/include)
include_directories(external/freetype/include)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
#Для игры копирование ресурсов из источника в билд
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

if (DEFINED EDITOR_BUILD)
	set(IMGUI_INCLUDES
	external/imgui/imgui.h
	external/imgui/backends/imgui_impl_opengl3.h
	external/imgui/backends/imgui_impl_glfw.h)
	
	set(IMGUI_SOURCES
	external/imgui/imgui.cpp
	external/imgui/backends/imgui_impl_opengl3.cpp
	external/imgui/backends/imgui_impl_glfw.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_demo.cpp)

	add_library(ImGUI STATIC
	${IMGUI_INCLUDES}
	${IMGUI_SOURCES})
	
	target_include_directories(ImGUI PUBLIC external)
	target_include_directories(ImGUI PRIVATE external/imgui)

	target_link_libraries(ImGUI glad glfw)

	target_link_libraries(${PROJECT_NAME} ImGUI)	
endif()