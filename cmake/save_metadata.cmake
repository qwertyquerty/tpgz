set(TPGZ_SAVE_META_GCN_DIR ${CMAKE_CURRENT_BINARY_DIR}/save_files)
set(TPGZ_SAVE_META_WII_DIR ${CMAKE_CURRENT_BINARY_DIR}/save_files_wii)
file(MAKE_DIRECTORY ${TPGZ_SAVE_META_GCN_DIR})
file(MAKE_DIRECTORY ${TPGZ_SAVE_META_WII_DIR})

function(tpgz_gen_save_meta script output_dir output_name)
    add_custom_command(
        OUTPUT ${output_dir}/${output_name}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/external/misc/${script} ${ARGN}
        WORKING_DIRECTORY ${output_dir}
        DEPENDS ${CMAKE_SOURCE_DIR}/external/misc/${script}
        VERBATIM)
endfunction()

tpgz_gen_save_meta(ad.py ${TPGZ_SAVE_META_GCN_DIR} ad.bin)
tpgz_gen_save_meta(ad.py ${TPGZ_SAVE_META_WII_DIR} ad.bin)

tpgz_gen_save_meta(any.py ${TPGZ_SAVE_META_GCN_DIR} any.bin)
tpgz_gen_save_meta(any.py ${TPGZ_SAVE_META_WII_DIR} any.bin)

tpgz_gen_save_meta(glitchless.py ${TPGZ_SAVE_META_GCN_DIR} glitchless.bin)
tpgz_gen_save_meta(glitchless.py ${TPGZ_SAVE_META_WII_DIR} glitchless.bin)

tpgz_gen_save_meta(nosq.py ${TPGZ_SAVE_META_GCN_DIR} nosq.bin -p GCN)
tpgz_gen_save_meta(nosq.py ${TPGZ_SAVE_META_WII_DIR} nosq.bin -p WII)

tpgz_gen_save_meta(anyb.py ${TPGZ_SAVE_META_GCN_DIR} any_bite.bin)
tpgz_gen_save_meta(anyb_wii.py ${TPGZ_SAVE_META_WII_DIR} any_bite.bin)

tpgz_gen_save_meta(hundo.py ${TPGZ_SAVE_META_GCN_DIR} hundo.bin -p GCN)
tpgz_gen_save_meta(hundo.py ${TPGZ_SAVE_META_WII_DIR} hundo.bin -p WII)

# Everything RomHack.toml needs for the platform currently being configured.
set(TPGZ_SAVE_META_FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/ad.bin
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/any.bin
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/glitchless.bin
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/nosq.bin
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/any_bite.bin
    ${CMAKE_CURRENT_BINARY_DIR}/${TPGZ_CFG_SAVE_META_PATH}/hundo.bin)

# Every save metadata file, for both platforms, regardless of which one is currently
# configured. These are cheap (small Python scripts, not real compilation), so there's no
# reason not to always build the full set as part of the default build.
set(TPGZ_SAVE_META_ALL_FILES
    ${TPGZ_SAVE_META_GCN_DIR}/ad.bin ${TPGZ_SAVE_META_WII_DIR}/ad.bin
    ${TPGZ_SAVE_META_GCN_DIR}/any.bin ${TPGZ_SAVE_META_WII_DIR}/any.bin
    ${TPGZ_SAVE_META_GCN_DIR}/glitchless.bin ${TPGZ_SAVE_META_WII_DIR}/glitchless.bin
    ${TPGZ_SAVE_META_GCN_DIR}/nosq.bin ${TPGZ_SAVE_META_WII_DIR}/nosq.bin
    ${TPGZ_SAVE_META_GCN_DIR}/any_bite.bin ${TPGZ_SAVE_META_WII_DIR}/any_bite.bin
    ${TPGZ_SAVE_META_GCN_DIR}/hundo.bin ${TPGZ_SAVE_META_WII_DIR}/hundo.bin)
add_custom_target(save_metadata ALL DEPENDS ${TPGZ_SAVE_META_ALL_FILES})
