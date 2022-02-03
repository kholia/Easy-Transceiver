# https://github.com/arduino/arduino-cli/releases

port := $(shell python3 board_detect.py)
# fqbn := --fqbn=arduino:avr:nano:cpu=atmega328old
fqbn := --fqbn=arduino:avr:nano:cpu=atmega328

default:
	arduino-cli compile ${fqbn} Easy-Transceiver
	# arduino-cli compile ${fqbn} usdx

upload:
	@# echo $(port)
	arduino-cli compile ${fqbn} Easy-Transceiver
	arduino-cli -v upload -p "${port}" ${fqbn} Easy-Transceiver

install_platform:
	arduino-cli core update-index
	arduino-cli core install arduino:avr

deps:
	arduino-cli lib install "Etherkit Si5351"

install_arduino_cli:
	curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh
