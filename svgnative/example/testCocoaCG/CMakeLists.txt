add_executable(testCocoaCG MACOSX_BUNDLE
    AppDelegate.h
    AppDelegate.mm
    Document.h
    Document.mm
    SVGNSView.h
    SVGNSView.mm
    main.mm
    MainMenu.xib
    Document.xib
)

set_source_files_properties(MainMenu.xib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
set_source_files_properties(Document.xib PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

set_target_properties(testCocoaCG PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/Info.plist)

target_link_libraries(testCocoaCG PUBLIC SVGNativeViewerLib)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AppKit")