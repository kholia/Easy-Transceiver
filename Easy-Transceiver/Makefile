format:
	astyle --options="formatter.conf" Easy-Transceiver.ino

lint:
	tidy data/index.html

run:
	# export WEAKMON=${PWD}/weakmon && python transceiver_control_for_manual_testing.py
	export WEAKMON=${PWD}/weakmon && python3 transceiver_server.py

get_deps:
	git clone https://github.com/rtmrtmrtmrtm/weakmon || true
	pip3 install -U -r requirements.txt
