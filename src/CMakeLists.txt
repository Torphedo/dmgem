add_executable(dmgem
	"main.c"
    "rom.c"
	"cpu.c"
	"bus.c"
	"machine.c"
	"arguments.c"
	"memory_controllers.c"
    "sm83_operations.c"
	"../external/logging/logging.c"
)

target_link_libraries(dmgem
		PRIVATE arena
)

target_include_directories(dmgem PRIVATE
		"../external"
		"../external/logging"
)

