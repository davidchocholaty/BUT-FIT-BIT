# Calculator Makefile

SHELL=/bin/bash
LOGINS=xchoch09_xbakaj00_xkanko00_xbalaz15
APP_NAME=kalkulator
SCRIPT_NAME=kalkulator
APP_MAIN=main
TEST_NAME=test_calc
DOXYGEN=doxygen
DOXYFILE=Doxyfile
SPEC=main
REPO=repo
CHANGELOG=changelog
COPYRIGHT=copyright
VENV=kivy_venv
REQ=requirements
PIP=pip
PIP3=pip3
PYTHON=python3
VERSION=1.0
PROFILE=cProfile
PROFILING_APP=profiling.py
PROFILING_IN_10=profiling_10.txt
PROFILING_IN_100=profiling_100.txt
PROFILING_IN_1000=profiling_1000.txt
PROFILING_IN_100000=profiling_100000.txt
PROFILING_OUT=../profiling/vystup.out

.PHONY: all pack clean test doc run profile zip deb-pckg executable install-python install-pip install-doxygen venv

all: profile run

pack: clean zip dep-pckg doc
	mkdir -p ${LOGINS}/doc ${LOGINS}/install ${LOGINS}/repo
	mv ${APP_NAME}-${VERSION}.deb ${LOGINS}/install
	mv ./doc/* ${LOGINS}/doc
	mv ./${REPO}.zip ${LOGINS}/repo && cd ${LOGINS}/repo && unzip ${REPO}.zip && rm -f ${REPO}.zip
	zip -r ${LOGINS}.zip ${LOGINS} && rm -rf ${LOGINS}

clean:	
	rm -rf ./build
	rm -rf ./dist
	rm -rf ./__pycache__
	rm -rf ./${APP_NAME}-${VERSION}
	rm -rf ./doc
	rm -rf kivy_venv
	rm -f ${LOGINS}.zip

test: install-python
	${PYTHON} ${TEST_NAME}.py

doc: install-doxygen
	doxygen ${DOXYFILE}

run: venv
	. ${VENV}/bin/activate && ${PIP3} install -r ${REQ}.txt && ${PYTHON} ${APP_MAIN}.py

profile: install-python
	@echo "Profiling output for file ${PROFILING_IN_10}     to file ${PROFILING_OUT} ..."

	@echo "*************** Vystup profilovani pro vstupni soubor ${PROFILING_IN_10} ***************\n" > ${PROFILING_OUT}
	@echo "Smerodatna odchylka pro vstupni soubor ${PROFILING_IN_10}:" >> ${PROFILING_OUT}
	@${PYTHON} -m ${PROFILE} ${PROFILING_APP} < ${PROFILING_IN_10} >> ${PROFILING_OUT}
	@echo "**************************************************************************************\n" >> ${PROFILING_OUT}



	@echo "Profiling output for file ${PROFILING_IN_100}    to file ${PROFILING_OUT} ..."

	@echo "*************** Vystup profilovani pro vstupni soubor ${PROFILING_IN_100} **************\n" >> ${PROFILING_OUT}
	@echo "Smerodatna odchylka pro vstupni soubor ${PROFILING_IN_100}:" >> ${PROFILING_OUT}
	@${PYTHON} -m ${PROFILE} ${PROFILING_APP} < ${PROFILING_IN_100} >> ${PROFILING_OUT}
	@echo "**************************************************************************************\n" >> ${PROFILING_OUT}

	

	@echo "Profiling output for file ${PROFILING_IN_1000}   to file ${PROFILING_OUT} ..."

	@echo "************** Vystup profilovani pro vstupni soubor ${PROFILING_IN_1000} **************\n" >> ${PROFILING_OUT}	
	@echo "Smerodatna odchylka pro vstupni soubor ${PROFILING_IN_1000}:" >> ${PROFILING_OUT}
	@${PYTHON} -m ${PROFILE} ${PROFILING_APP} < ${PROFILING_IN_1000} >> ${PROFILING_OUT}
	@echo "**************************************************************************************\n" >> ${PROFILING_OUT}

	

	@echo "Profiling output for file ${PROFILING_IN_100000} to file ${PROFILING_OUT} ..."

	@echo "************* Vystup profilovani pro vstupni soubor ${PROFILING_IN_100000} *************\n" >> ${PROFILING_OUT}
	@echo "Smerodatna odchylka pro vstupni soubor ${PROFILING_IN_100000}:" >> ${PROFILING_OUT}
	@${PYTHON} -m ${PROFILE} ${PROFILING_APP} < ${PROFILING_IN_100000} >> ${PROFILING_OUT}
	@echo "**************************************************************************************\n" >> ${PROFILING_OUT}

zip:
	cd .. && zip -r src/${REPO}.zip * .git .gitignore

dep-pckg: executable
	mkdir -p ./$(APP_NAME)-$(VERSION)/usr/bin
	mkdir -p ./$(APP_NAME)-$(VERSION)/opt/$(APP_NAME)
	mkdir -p ./$(APP_NAME)-$(VERSION)/usr/share/applications
	mkdir -p ./$(APP_NAME)-$(VERSION)/usr/share/doc/$(APP_NAME)
	mv ./dist/* ./$(APP_NAME)-$(VERSION)/opt/$(APP_NAME)
	cp ./install/$(SCRIPT_NAME) ./$(APP_NAME)-$(VERSION)/usr/bin
	chmod a+x ./$(APP_NAME)-$(VERSION)/usr/bin/$(SCRIPT_NAME)
	cp -r ./DEBIAN ./$(APP_NAME)-$(VERSION)
	mv ./$(APP_NAME)-$(VERSION)/DEBIAN/${CHANGELOG}.gz ./$(APP_NAME)-$(VERSION)/usr/share/doc/$(APP_NAME)
	mv ./$(APP_NAME)-$(VERSION)/DEBIAN/${COPYRIGHT} ./$(APP_NAME)-$(VERSION)/usr/share/doc/$(APP_NAME)
	cp $(APP_NAME).desktop ./$(APP_NAME)-$(VERSION)/usr/share/applications
	dpkg-deb --build --root-owner-group $(APP_NAME)-$(VERSION)

executable: venv
	. ${VENV}/bin/activate && ${PIP3} install -r ${REQ}.txt && pyinstaller ${SPEC}.spec

install-python:
ifeq (, $(shell which ${PYTHON} ))
	sudo apt install ${PYTHON}
endif

install-pip:
ifeq (, $(shell which ${PIP} ))
	sudo apt install ${PYTHON}-${PIP}
endif

install-doxygen:
ifeq (, $(shell which ${DOXYGEN} ))
	sudo apt install ${DOXYGEN}
endif

venv: install-python install-pip	
	${PYTHON} -m ${PIP} install --upgrade ${PIP} setuptools virtualenv
	test -d ${VENV} || ${PYTHON} -m virtualenv ${VENV}