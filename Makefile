#ESPHOME = docker run --rm -p 6052:6052 -e ESPHOME_DASHBOARD_USE_PING=true -v "${PWD}":/config -v "${PWD}/.esphome/platformio":/root/.platformio -it ghcr.io/esphome/esphome
ESPHOME = esphome
YTE = .venv/bin/yglu
TPL = esp32-s3-box-3-5aac68.yglu.yaml
YML = esp32-s3-box-3-5aac68.yaml

.venv: requirements.txt
	rm -rf .venv
	python3 -m venv .venv
	.venv/bin/pip install -r requirements.txt

.PHONY: run upload logs clean 
$(YML): .venv $(TPL)
	$(YTE) < $(TPL) > $(YML).tmp
	mv $(YML).tmp $(YML)
	rm -f $(YML).tmp

run: $(YML)
	$(ESPHOME) run $(YML)

upload: $(YML)
	$(ESPHOME) upload $(YML)

compile: $(YML)
	$(ESPHOME) compile $(YML)

logs: $(YML)
	$(ESPHOME) logs $(YML)

clean: $(YML)
	rm -rf .esphome/


.esphome/test/layout.test.yaml: layout.test.yglu.yaml
	mkdir -p .esphome/test
	$(YTE) < layout.test.yglu.yaml > .esphome/test/layout.test.yaml.tmp
	mv .esphome/test/layout.test.yaml.tmp .esphome/test/layout.test.yaml
	rm -f .esphome/test/layout.test.yaml.tmp

.esphome/test/secrets.yaml: secrets.yaml
	mkdir -p .esphome/test
	cp -p secrets.yaml .esphome/test/secrets.yaml

test: .esphome/test/layout.test.yaml .esphome/test/secrets.yaml
	$(ESPHOME) compile .esphome/test/layout.test.yaml

