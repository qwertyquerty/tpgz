# Custom target that will compile all the modules at once
add_custom_target(modules)

set_property(TARGET modules PROPERTY "TPGZ_MODULE_LAST_ID" 4096)
set(TPGZ_MODULE_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/rels)
set(TPGZ_MODULE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

make_directory(${TPGZ_MODULE_OUT_DIR})

# Get source from common/rels. They are sources that have to be compiled with every REL module.
file(GLOB_RECURSE TPGZ_EXTERNAL_REL_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/common/rels/src/*.cpp")

list(APPEND TPGZ_EXTERNAL_REL_SRC ${TPGZ_EXTERNAL_REL_CPP})
if(PLATFORM STREQUAL "GCN")
    list(APPEND TPGZ_EXTERNAL_REL_SRC ${CMAKE_SOURCE_DIR}/tp/libs/PowerPC_EABI_Support/Runtime/Src/global_destructor_chain.c)
else()
    list(APPEND TPGZ_EXTERNAL_REL_SRC ${CMAKE_SOURCE_DIR}/common/rels/src/runtime_wii.c)
endif()
set(TPGZ_REL_LCF ${CMAKE_SOURCE_DIR}/common/rels/src/rel.lcf)

include_directories(BEFORE ${CMAKE_SOURCE_DIR}/external/)
include_directories(BEFORE ${CMAKE_SOURCE_DIR}/common/)

set(TPGZ_SYMBOL_MAP ${CMAKE_CURRENT_BINARY_DIR}/${PLATFORM}_${REGION}.lst)
set(TPGZ_DECOMP_CONFIG_DIR ${CMAKE_SOURCE_DIR}/tp/config)
set(TPGZ_REL_IDS ${CMAKE_SOURCE_DIR}/res/map/rel_ids/${PLATFORM}_${REGION}.lst)
file(GLOB TPGZ_DECOMP_REL_SYMBOLS CONFIGURE_DEPENDS "${TPGZ_DECOMP_CONFIG_DIR}/${TPGZ_DECOMP_ASSET_VERSION}/rels/*/symbols.txt")
if(EXISTS ${TPGZ_REL_IDS})
    set(TPGZ_REL_IDS_ARGS --rel-ids ${TPGZ_REL_IDS})
    set(TPGZ_REL_IDS_DEPENDS ${TPGZ_REL_IDS})
endif()
add_custom_command(OUTPUT ${TPGZ_SYMBOL_MAP}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bin/merge_lst.py
        ${TPGZ_SYMBOL_MAP} ${TPGZ_DECOMP_CONFIG_DIR} ${TPGZ_DECOMP_ASSET_VERSION} ${TPGZ_REL_IDS_ARGS}
    DEPENDS ${TPGZ_DECOMP_CONFIG_DIR}/${TPGZ_DECOMP_ASSET_VERSION}/config.yml
        ${TPGZ_DECOMP_CONFIG_DIR}/${TPGZ_DECOMP_ASSET_VERSION}/symbols.txt
        ${TPGZ_DECOMP_REL_SYMBOLS} ${TPGZ_REL_IDS_DEPENDS}
        ${CMAKE_SOURCE_DIR}/bin/merge_lst.py)
add_custom_target(tpgz_symbol_map DEPENDS ${TPGZ_SYMBOL_MAP})

set(TPGZ_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
make_directory(${TPGZ_GENERATED_DIR})
set(TPGZ_SAVESTATE_RANGES ${TPGZ_GENERATED_DIR}/savestate_ranges.h)
add_custom_command(OUTPUT ${TPGZ_SAVESTATE_RANGES}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bin/gen_savestate_ranges.py
        ${TPGZ_SAVESTATE_RANGES} ${TPGZ_DECOMP_CONFIG_DIR}/${TPGZ_DECOMP_ASSET_VERSION}/splits.txt
    DEPENDS ${TPGZ_DECOMP_CONFIG_DIR}/${TPGZ_DECOMP_ASSET_VERSION}/splits.txt
        ${CMAKE_SOURCE_DIR}/bin/gen_savestate_ranges.py)
add_custom_target(tpgz_savestate_ranges DEPENDS ${TPGZ_SAVESTATE_RANGES})

# Definition of the functions which sets up REL modules
function(tpgz_add_main_module module_name module_srcs)
    if(ARGC GREATER "2")
        set(module_inc_paths ${ARGV2})
    endif()
    if(ARGC GREATER "3")
        set(module_lib_paths ${ARGV3})
    endif()

    get_target_property(TPGZ_MODULE_ID_COUNTER modules TPGZ_MODULE_LAST_ID)

    add_executable(${module_name} EXCLUDE_FROM_ALL)
    target_sources(${module_name}
        PUBLIC ${module_srcs} ${TPGZ_EXTERNAL_REL_SRC})
    set_property(TARGET ${module_name} APPEND PROPERTY LINK_OPTIONS "SHELL:-map ${module_name}.map")
    set_property(TARGET ${module_name} APPEND PROPERTY LINK_DEPENDS ${TPGZ_REL_LCF})

    if(module_inc_paths)
        target_include_directories(${module_name} BEFORE PUBLIC ${module_inc_paths})
    endif()
    if(module_lib_paths)
        target_link_directories(${module_name} ${module_lib_paths})
    endif()

    file(GLOB relative_module_name LIST_DIRECTORIES true RELATIVE ${TPGZ_MODULE_SRC_DIR} CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_command(OUTPUT ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel
        DEPENDS ${module_name} ${TPGZ_SYMBOL_MAP}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bin/run_elf2rel.py ${TPGZ_ELF2REL_EXE} $<TARGET_FILE:${module_name}> -s ${TPGZ_SYMBOL_MAP} --rel-id ${TPGZ_MODULE_ID_COUNTER} -o ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.lst
        DEPENDS ${module_name} ${TPGZ_SYMBOL_MAP}
        COMMAND ${Python3_EXECUTABLE} ${TPGZ_RELMAPPER_EXE} -o ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.lst -i ${TPGZ_SYMBOL_MAP} -s ${TPGZ_MODULE_ID_COUNTER} $<TARGET_FILE:${module_name}>)
    add_custom_target(${module_name}_rel
        DEPENDS ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.lst)
    add_dependencies(${module_name}_rel tpgz_symbol_map)
    set_property(TARGET ${module_name}_rel PROPERTY TPGZ_MODULE_FILE ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel)

    set_property(TARGET modules PROPERTY TPGZ_MAIN_MODULE_LST ${CMAKE_CURRENT_BINARY_DIR}/${module_name}.lst)
    set_property(TARGET modules PROPERTY TPGZ_MAIN_MODULE_TRG ${module_name}_rel)
    set_property(TARGET modules PROPERTY TPGZ_MAIN_MODULE_INC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_dependencies(modules ${module_name}_rel)
    set_property(TARGET modules APPEND PROPERTY TPGZ_MODULES ${module_name})

    math(EXPR TPGZ_MODULE_ID_COUNTER "${TPGZ_MODULE_ID_COUNTER}+1")
    set_property(TARGET modules PROPERTY "TPGZ_MODULE_LAST_ID" ${TPGZ_MODULE_ID_COUNTER})
endfunction()

# This function will setup an individual module to be compiled and added to the dependencies of the "modules" target
function(tpgz_add_module module_name module_srcs)
    if(ARGC GREATER "2")
        set(module_inc_paths ${ARGV2})
    endif()
    if(ARGC GREATER "3")
        set(module_lib_paths ${ARGV3})
    endif()

    get_target_property(TPGZ_MODULE_ID_COUNTER modules TPGZ_MODULE_LAST_ID)
    get_target_property(TPGZ_MAIN_MODULE_LST modules TPGZ_MAIN_MODULE_LST)
    get_target_property(TPGZ_MAIN_MODULE_TRG modules TPGZ_MAIN_MODULE_TRG)
    get_target_property(TPGZ_MAIN_MODULE_INC modules TPGZ_MAIN_MODULE_INC)

    add_executable(${module_name} EXCLUDE_FROM_ALL)
    target_sources(${module_name}
        PUBLIC ${module_srcs} ${TPGZ_EXTERNAL_REL_SRC})
    set_property(TARGET ${module_name} APPEND PROPERTY LINK_OPTIONS "SHELL:-map ${module_name}.map" -strip_partial)
    set_property(TARGET ${module_name} APPEND PROPERTY LINK_DEPENDS ${TPGZ_REL_LCF})

    if(module_inc_paths)
        target_include_directories(${module_name} BEFORE PUBLIC ${module_inc_paths})
    endif()
    target_include_directories(${module_name} BEFORE PUBLIC ${TPGZ_MAIN_MODULE_INC})
    if(module_lib_paths)
        target_link_directories(${module_name} ${module_lib_paths})
    endif()

    file(GLOB relative_module_name LIST_DIRECTORIES true RELATIVE ${TPGZ_MODULE_SRC_DIR} CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_command(OUTPUT ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel
        DEPENDS ${module_name} ${TPGZ_MAIN_MODULE_TRG} ${TPGZ_MAIN_MODULE_LST}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bin/run_elf2rel.py ${TPGZ_ELF2REL_EXE} $<TARGET_FILE:${module_name}> -s ${TPGZ_MAIN_MODULE_LST} --rel-id ${TPGZ_MODULE_ID_COUNTER} -o ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel)
    add_custom_target(${module_name}_rel
        DEPENDS ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel)
    add_dependencies(${module_name}_rel tpgz_symbol_map)
    set_property(TARGET ${module_name}_rel PROPERTY TPGZ_MODULE_FILE ${TPGZ_MODULE_OUT_DIR}/${module_name}.rel)

    add_dependencies(modules ${module_name}_rel)
    set_property(TARGET modules APPEND PROPERTY TPGZ_MODULES ${module_name})

    math(EXPR TPGZ_MODULE_ID_COUNTER "${TPGZ_MODULE_ID_COUNTER}+1")
    set_property(TARGET modules PROPERTY "TPGZ_MODULE_LAST_ID" ${TPGZ_MODULE_ID_COUNTER})
endfunction()

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DREL_MODULE)

set(TPGZ_MW_CFLAGS
    -nodefaults "SHELL:-proc gekko" "SHELL:-align powerpc" "SHELL:-enum int" "SHELL:-fp hardware" "SHELL:-Cpp_exceptions off"
    "SHELL:-pragma \"cats off\"" "SHELL:-pragma \"warn_notinlined off\"" "SHELL:-maxerrors 1" -nosyspath "SHELL:-str reuse"
    "SHELL:-use_lmw_stmw off" "SHELL:-schedule off" "SHELL:-sym on" "SHELL:-fp_contract off" "SHELL:-sdata 0" "SHELL:-sdata2 0")
if(PLATFORM STREQUAL "GCN")
    list(APPEND TPGZ_MW_CFLAGS -multibyte "SHELL:-RTTI off" "SHELL:-inline noauto" -O3,s "SHELL:-str reuse,pool,readonly")
else()
    list(APPEND TPGZ_MW_CFLAGS "SHELL:-enc SJIS" -D__REVOLUTION_SDK__ -DSDK_SEP2006 -DWIDESCREEN_SUPPORT=1 "SHELL:-ipa file" "SHELL:-RTTI on" "SHELL:-inline noauto" -O4,s)
endif()
add_compile_options(${TPGZ_MW_CFLAGS})
add_link_options("SHELL:-fp hardware" -nodefaults "SHELL:-sdata 0" "SHELL:-sdata2 0" -r1 "SHELL:-lcf ${TPGZ_REL_LCF}" "SHELL:-m _prolog")

file(GLOB TPGZ_SUB_MODULES CONFIGURE_DEPENDS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")

list(REMOVE_ITEM TPGZ_SUB_MODULES boot)
message(VERBOSE "Including main module 'boot'")
add_subdirectory(boot)

# Include all the submodules
foreach(module ${TPGZ_SUB_MODULES})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${module})
        message(VERBOSE "Including submodule '${module}'")
        add_subdirectory(${module})
    endif()
endforeach()
