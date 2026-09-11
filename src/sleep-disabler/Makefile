BINARY_INSTALL_DIR = $$HOME/.local/sleep-disabler/bin

all: disable-sleep enable-sleep

enable-sleep: disable-sleep
	ln -f $< $@

disable-sleep: disable-sleep.c
	cc -o $@ $<

install: all
	>&2 echo "Installing disable-sleep and enable-sleep in '$(BINARY_INSTALL_DIR)'."
	mkdir -p $(BINARY_INSTALL_DIR)
	ln -f disable-sleep "$(BINARY_INSTALL_DIR)/disable-sleep"
	# Use a hard link so that they are the same file and we only need to setuid
	# once.
	ln -f $(BINARY_INSTALL_DIR)/disable-sleep $(BINARY_INSTALL_DIR)/enable-sleep

clean:
	rm -f "$(BINARY_INSTALL_DIR)/disable-sleep" "$(BINARY_INSTALL_DIR)/enable-sleep" disable-sleep enable-sleep
