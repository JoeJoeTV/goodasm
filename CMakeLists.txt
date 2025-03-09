cmake_minimum_required(VERSION 3.16)

project(goodasm VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)


set(GOODASM_SOURCES
    # GoodASM classes
    gainstruction.h gainstruction.cpp
    gamnemonic.h gamnemonic.cpp
    gaparameter.h gaparameter.cpp
    goodasm.h goodasm.cpp
    gasymboltable.h gasymboltable.cpp

    # Listing Styles
    galisting.h galisting.cpp
    galistingdefault.h galistingdefault.cpp
    galistingnasm.h galistingnasm.cpp
    galistingc.h galistingc.cpp
    galistinggo.h galistinggo.cpp
    galistingyarax.h galistingyarax.cpp
    galistingmarkdown.h galistingmarkdown.cpp
    galistinghex.h galistinghex.cpp
    galistingga.h galistingga.cpp

    # Lexer and Parser
    gatoken.h gatoken.cpp
    galexer.h galexer.cpp
    gaparser.h gaparser.cpp

    # Interactive mode.
    garepl.h garepl.cpp

    # Languages
    galanguage.h galanguage.cpp
    galangucom43.h galangucom43.cpp
    galangtlcs47.h galangtlcs47.cpp
    galangs2000.h galangs2000.cpp
    galangpic16c5x.h galangpic16c5x.cpp
    galangmarc4.h galangmarc4.cpp
    galang6502.h galang6502.cpp
    galang8051.h galang8051.cpp
    galangsm83.h galangsm83.cpp
    galang6805.h galang6805.cpp
    galangchip8.h galangchip8.cpp
    galangz80.h galangz80.cpp
    galang8086.h galang8086.cpp
    galangfcard.h galangfcard.cpp
)


# We don't build the GUI on Linux, as Ubuntu ships
# older versions of Qt Widgets and we aren't compatible.
if(ANDROID OR IOS)
  qt_add_executable(appgoodasmapp
      maingui.cpp
      ${GOODASM_SOURCES}
  )

  qt_add_qml_module(appgoodasmapp
      URI goodasmapp
      VERSION 1.0
      QML_FILES
          Main.qml
          SOURCES gaobject.h gaobject.cpp
  )

  # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
  # If you are developing for iOS or macOS you should consider setting an
  # explicit, fixed bundle identifier manually though.
  set_target_properties(appgoodasmapp PROPERTIES
  #    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appgoodasmapp
      MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
      MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
      MACOSX_BUNDLE TRUE
      WIN32_EXECUTABLE TRUE
  )

  # Readline is only used on Unix.
  if(WIN32 OR IOS OR ANDROID)
    target_link_libraries(appgoodasmapp PRIVATE Qt6::Quick)
  else()
    target_link_libraries(appgoodasmapp PRIVATE Qt6::Quick readline)
  endif()

  set(APPS
    appgoodasmapp)
else()
  set(APPS
    goodasm)
endif()


include(GNUInstallDirs)

# This defines the CLI client.
if(WIN32 OR (LINUX AND NOT ANDROID) OR (APPLE AND NOT IOS) )
    qt_add_executable(goodasm
        main.cpp
        ${GOODASM_SOURCES}
    )
    qt_add_library(libgoodasm STATIC
        #main.cpp
        ${GOODASM_SOURCES}
    )
    target_link_libraries(libgoodasm PRIVATE Qt6::Quick)
    if(WIN32)
        target_link_libraries(goodasm PRIVATE Qt6::Quick)
    else()
        target_link_libraries(goodasm PRIVATE Qt6::Quick readline)
    endif()

    install(TARGETS ${APPS} DESTINATION bin)
endif()


install(TARGETS ${APPS}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
