# w pliku `local.mk` zdefiniuj zmienną SFML_DIR ze ścierzką do SFML'a!
# jest ona różna dla różnych komputerów, więc nie powinna być commitowana, tylko w innym pliku wisieć
include local.mk

###### Definicja pomocniczych zmiennych #######

# kompilator
CXX := g++
# domyślna konfiguracja (można ją zoverrideować wywołując podczas uruchomienaia "make" z konsoli: np. `make config=release`)
# (dowolną zmienną można tak zoverrideować, jeśli jest użyty przy przypisaniu ":=". Jakbym użył zwykłego "=" w Makefile'u, to Makefile wziął priorytet)
config := debug

# Przypisanie nazw wszystkich plików z rozszerzeniem *.cpp w aktualnym folderze do zmiennej
# (jak chcesz, możnesz też je ręcznie wypisać/dopisać)
SOURCE_FILES := $(wildcard *.cpp)
# Lista folderów do przekopiowania do wyjściowego folderu
ASSETS_FILES := Fonts Images Sounds

## Ścierzki do przydatnych folderów ##
# folder z wyjściowym programem, gotowym do uruchomienia
INSTALL_DIR := ./dist/${config}
# folder z plikami przejściowymi, potrzebnymi do kompilacji
BUILD_DIR   := ./build/${config}

# nazwa głównego pliku wyjściowego
TARGET      := ${INSTALL_DIR}/chess.exe

# Dodatkowe flagi do kompilacji i linkowania (domyślnie puste, potem będzie do tych zmiennych dopisywane)
CPP_COMPILE_FLAGS :=
CPP_LINK_FLAGS    :=

# Dodatkowe opcje, specyficzne tylko dla konfiguracji "debug"
ifeq (${config},debug)
endif

# Dodatkowe opcje, specyficzne tylko dla konfiguracji "release"
ifeq (${config},release)
    # Dopisanie (+=) do listy CPP_COMPILE_FLAGS nowej flagi, włączającej optymalizację
    CPP_COMPILE_FLAGS += -O3
endif

# (możnesz se dodać więcej konfiguracji, zmodyfikowac dowolną zmienną dla dowolnej z nich, itd.)

################# SFML #################

SFML_INCLUDE := ${SFML_DIR}/include
SFML_LIB     := ${SFML_DIR}/lib

# dodanie flag kompilatora i linkera z SFML'a
CPP_COMPILE_FLAGS += -I${SFML_INCLUDE}
CPP_LINK_FLAGS    += -L${SFML_LIB} -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

################# BUILD FOLDERS #################

 # folder na pliki pomocnicze, które będą częściowo określały, który plik zależy od którego
DEPS_DIR := ${BUILD_DIR}/deps
 # folder na pliki wyjściowe kompilacji przed zlinkowaniem (*.o)
OBJ_DIR  := ${BUILD_DIR}/obj

# Zasttosowanie specialnej składni, która podmnieani i/lub dodaje stringi w liście stringów
#   W tym przypadku, podmieniam folder i rozszerzenie plików ".cpp" (SOURCE_FILES), żeby je przerobić na ".o" (przed linkowaniem) 
#   i ".d" (automatycznie wygenerowane pliki przechowujące zależności między użytymi .cpp a .hpp)
DEPS_FILES  := ${SOURCE_FILES:%.cpp=${DEPS_DIR}/%.d}
OBJ_FILES   := ${SOURCE_FILES:%.cpp=${OBJ_DIR}/%.o}
# Tutaj dodaję prefix do każdego folderu assetów, żeby folder docelowy po instalacji był dobry
INSTALLED_ASSETS := ${ASSETS_FILES:%=${INSTALL_DIR}/%}

################# HELPER FUNCTIONS #################
# Pomocnicze funkcje.
# Wywwołuje je się składnią $(call <nazwa funkcji>,<lista argumentów>)
# Argumenty oznaczane są jako $1, $2, itd...

# Podmiana slashów w ścierzce, żeby windows się nie czepiał
define winpath
$(subst /,\,$1)
endef

# kopiowanie plików i folderów do folderu ${INSTALL_DIR}
define copytoinstall
xcopy $(call winpath,$1) $(call winpath,${INSTALL_DIR}/$1) /Y /E /I
endef

# tworzenie struktury folderów
define mkdir
if not exist $(call winpath,$1) mkdir $(call winpath,$1)
endef

# Usuwanie folderów
define rmdir
rmdir /S /Q $(call winpath,$1)
endef

################# TARGETS #################

# Tu taka specialna składnia, określająca, że te targety są wirtualne, nie są to konkretne pliki
.PHONY: default build_and_install build install run clean dir_structure
# domyślnie ma sie wykonać target "build_and_install" 
default: build_and_install

# Stworzenie nowej reguły
# element ${TARGET} jest zależny od tergetów w liście ${OBJ_FILES} oraz targetu "dir_structure"
${TARGET}: ${OBJ_FILES} | dir_structure
# Jeśli którakolwiek z dependencies (nie licząc tych po znaku "|", czyli dir_structure) zostanie zaktualizowana,
# to uruchamiana jest poniższa komenda.
# Pod każdą zmienną podstawiana jest jej wartość i uruchamiane jest w CMD
	${CXX} ${OBJ_FILES} -o ${TARGET} ${CPP_LINK_FLAGS}

# Ta reguła automatycznie generuje wiele reguł, dla każdej z listy ${OBJ_FILES}
# Dla każdej z nich, jej nazwa jest parsowana, i pod "%" podstawiany jest string taki, żeby pasował
#  (np. dla elementu listy ${OBJ_FILES} o wartości "./build/debug/chess.o" za "%" podstawione będzie "chess" )
${OBJ_FILES}: ${OBJ_DIR}/%.o: %.cpp | dir_structure
# W komendzie CMD nie można użyć podstawienia "%", tutaj to co się podstawiło trzeba wyciągnąć za pomocą znaku "$*"
# Ta końcówka "-MMD -MP -MF ${DEPS_DIR}/$*.d" mówi GCC, żeby wygenerował automatycznie plik, z diagramem od jakich .hpp zależy ten .cpp
	${CXX} $*.cpp -o ${OBJ_DIR}/$*.o ${CPP_COMPILE_FLAGS} -c -MMD -MP -MF ${DEPS_DIR}/$*.d

# Tutaj importujemy reguły wygenerowane za pomocą "-MMD -MP -MF ${DEPS_DIR}/$*.d"
# Gdyby tego nie zrobić, to Make nie zauważyłby zmian w plikach .hpp, bo nie wiedział by, które od czego zależą
-include ${DEPS_FILES}


# wirtualny target "build" jest zależny od ${TARGET}
build: ${TARGET}

 # wirtualny target "install", który zależy od assetów w folderze docelowym
install: ${INSTALLED_ASSETS}

build_and_install: build install 

# Reguła, że każdy element z listy ${INSTALLED_ASSETS} powinien zostać zbudowany, za pomocą przekopiowania odpowiedniego folderu
${INSTALLED_ASSETS}: ${INSTALL_DIR}/%: ./%
	$(call copytoinstall,$*)

# Reguła, która po prostu uruchamia wynikowy .exe
run:
	${TARGET}

# Usunięcie całego folderu z artefaktami kompilacji
clean:
	@$(call rmdir,${BUILD_DIR})

# Reguła tworząca strukturę folderów
dir_structure: 
	@$(call mkdir,${BUILD_DIR})
	@$(call mkdir,${DEPS_DIR})
	@$(call mkdir,${OBJ_DIR})
	@$(call mkdir,${INSTALL_DIR})


######### HOW TO USE #########
# Wywołaj w cmd polecenie:
# "make" - wykonanie domyślnej reguły (czyli build_and_install), z domyślną konfiguracją (czyli debug)
# "make config=release" - to samo, ale z inną konfiguracją
# "make -j8" - uruchomienie reguły na 8 wątkach naraz
# "make build" - wykonanie konkretnej reguły (w tym przypadku sam build, bez install)
# "make run config=release" - wykonanie run, dla konfiguracji release
# itd...

# !!! DLL'ki uznałem, ze sam se musisz przekopiować do odpowiednich podfolderów w "dist" !!!