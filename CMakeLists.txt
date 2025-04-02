# Wstępna konfiguracja projektu, wymagana w każdym cmake'u
cmake_minimum_required(VERSION 3.24)
project(Chess)

# Włączenie flagi, żeby output gcc był kolorowany, ładniej tak wygląda
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Konfiguracja (tutaj są podane wartości domyślne, żeby je zmienić, to trzeba zmodyfikować plik "build/CMakeCache.txt", albo komendą w cmd)
option(OPTION_SFML_STATIC  "Link SFML staticaly" ON)
option(OPTION_STDCPP_STATIC "Link standard c++ library statically" ON)

# Ustawia zmienne "assets_folders" i "assets_files" na liste folderów i plików z assetami
# CMake musi wiedzieć, czy kopiuje plik czy folder, więc muszą one być w 2 listach różnych zdefiniowane
# Jak dodasz jakiś nowy folder albo plik, to trzeba tutaj też go dodać, żeby się automatycznie kopiował komendą "install" poniżej
set(assets_folders 
    Fonts Images Sounds
)
set(assets_files 
    check.frag
)

############## Chess   #################

# Przypisanie wszystkich plików z *.cpp w aktualnym folderze do zmiennej "sources_list"
file(GLOB sources_list CONFIGURE_DEPENDS "./*.cpp")

# Stworzenie Tergetu o nazwie "chess", będącego plikiem wykonywalnym, z plikami źródłowymi ze zmiennej "sources_list"
add_executable(chess ${sources_list})

# Jeśli jest ustawiona zmienna "OPTION_STDCPP_STATIC", i kożystasz z kompilatora GNU (GCC, Clang, itd.), to dodaj flagi do linkowania statycznego
if(OPTION_STDCPP_STATIC AND (GNU OR MINGW))
    target_link_options(chess PRIVATE -static -static-libgcc -static-libstdc++)
endif()

############## SFML    #################

# Automatyczne przeszukanie komputera, w poszukiwaniu SFML'a w wersji 3.0 i zaimportowanie wylistowanych modułów
# SFML w pliku "SFML-3.0.0/lib/cmake/SFML/SFMLConfig.cmake" definuje jakie DLL i flagi który komponent powinien dodać do projektu
# CMake znajduje ten plik i załatwia to wszystko sprytnie i automatycznie
set(SFML_STATIC_LIBRARIES ${OPTION_SFML_STATIC})
find_package(SFML 3.0 REQUIRED COMPONENTS System Window Graphics Audio)

# Zaimportowane komponenty trzeba teraz powiązać z naszym Targetem "chess"
target_link_libraries(chess 
    PRIVATE 
        SFML::System SFML::Window SFML::Graphics SFML::Audio
)

############## Install #################

# Ustawienie, żeby nie zaśmiecało cmd niepotrzebnymi logami
set(CMAKE_INSTALL_MESSAGE LAZY)

# To instaluje wynikowy exe bezpośrednio w docelowym folderze
install(TARGETS
    chess 
    DESTINATION .
)
# To kopiuje zdefinowane assety w odpowiednich zmiennych do folderu docelowego
install(DIRECTORY 
    ${assets_folders}
    DESTINATION .
)
install(FILES 
    ${assets_files}
    DESTINATION .
)
# To to samo, ale z DLL'kami z SFML'a (o ile linkujemy go dynamicznie)
if(NOT SFML_STATIC_LIBRARIES)
    install(IMPORTED_RUNTIME_ARTIFACTS 
        SFML::System SFML::Window SFML::Graphics SFML::Audio 
        DESTINATION .
    )
endif()

# To zainstaluje wszystkie pozostałe dll'ki, które są widocznie w PATH'ie (za wyjątkiem systemowych z "system32")
if(MINGW)
    install(CODE "
        set(CMAKE_INSTALL_MESSAGE LAZY)
        file(GET_RUNTIME_DEPENDENCIES
            EXECUTABLES \${CMAKE_INSTALL_PREFIX}/$<TARGET_FILE_NAME:chess>
            RESOLVED_DEPENDENCIES_VAR dlls
            UNRESOLVED_DEPENDENCIES_VAR notdlls
            PRE_EXCLUDE_REGEXES \"^api-ms-\" \"^ext-ms-\"
            POST_EXCLUDE_REGEXES \".*system32/.*\"
            DIRECTORIES \$ENV{PATH}
        )
        if(dlls)
            file(INSTALL \${dlls} DESTINATION \${CMAKE_INSTALL_PREFIX})
        endif()
    ")
endif()

############## Automatic Install and Run #################

# Tutaj zdefiniowałem kilka customowych komend, żeby można jedną komendą skompilować, zainstalować, i uruchomić program
# Normalnie trzeba by to trzema osobnymi komendami zawsze robić, i podawać od ręki ścierzkę instalacji w cmd 

# Zmienna ze ścierzką do docelowego folderu (jest zależna od konfiguracji, czyli od Debug, Release itd.)
set(dist_dir ${CMAKE_SOURCE_DIR}/dist/$<CONFIG>)

# Stworzenie customowego targetu, który buduje i instaluje projekt w folderze ${dist_dir}
add_custom_target(install_dist
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --config $<CONFIG> --prefix ${dist_dir}
    DEPENDS chess
)

# Stworzenie customowego targetu, który uruchamia projekt (po instalacji targetem "install_dist" liniję wyżej)
add_custom_target(run
    COMMAND $<TARGET_FILE_NAME:chess>
    WORKING_DIRECTORY ${dist_dir}
    DEPENDS install_dist
)

########################################
############## HOW TO USE ##############
#
#    1. Ustawienie konfiguracji cmake'a
#       Trzeba podać 3 rzeczy:
#           - nazwę folderu, w którym CMake będzie trzymał swoją konfigurację (zazwyczaj nazywa sie go "build")
#           - nazwę generatora. Domyślnie na Windowsie jest to "Visual Studio", ale jak masz tego pobranego już gotowego SFML'a, to go trzeba GCC skompilować, czego Visual Studio nie wspiera
#             Najpopularniejszy wspierający GCC generator to "Ninja Multi-Config". Ninja to taki "Make", ale szybszy, zainstalowany domyślnie razem z GCC
#           - ścierzkę, gdzie CMake ma szukać paczek (jak "SFML"). Jeśli nie masz swojego SFML'a w PATH'ie, ani w Program Files, to go CMake domyślnie nie znajdzie, 
#             i musisz albo zrobić zmienną środowiskową "CMAKE_PREFIX_PATH" ze ścierzką doń, albo podać ją w komendzie, jak napisałem poniżej
#
#       !!! Tą komendę uruchamiasz tylko raz, przy pierwszym uruchomieniu cmake'a !!!
#
#             cmake -B build -G "Ninja Multi-Config" -D CMAKE_PREFIX_PATH="<ścierzka do folderu SFML>"
#
#    2. Wszystkie inne komendy uruchamiać będziesz z wewnątrz folderu który cmake stworzył, o nazwie "build"
#       Należy do niego przejść
#
#             cd build
#
#   3. Żeby zkompilować projekt:
#      ( Nie zapomnij o tej kropce na końcu, i o tym by być w folderze "build", bo inaczej ci CMake naśmieci !! )
#
#             cmake --build .
#
#      Jeśli chcesz od razu uruchomić aplikację bo zbudowaniu, to musisz podać jeden z moich customowych targetów:
#
#             cmake --build . --target run
#
#      albo, jak chcesz tylko zbudować z przekopiowaniem DLLów i assetów, ale bez uruchamiania:
#
#             cmake --build . --target install_dist
#
#      Możesz też wybrać inną konfigurację (domyślnie jest Debug, ale może też być Release)
#
#             cmake --build . --target run --config Release
#      
#
#   4. Na samym początku tego pliku, definiuję opcje OPTION_SFML_STATIC i OPTION_STDCPP_STATIC
#      Jak chcesz zbudować bez statycznego linkowania, to musisz je ustawić na OFF.
#      W tym pliku są wartości domyślne tych opcji, jak chesz je zmienić, powinieneś to zrobić w pliku "build/CMakeCache.txt"
#      Tam je znajdź (CTRL+F) i podmień na chcianą wartość.
#




