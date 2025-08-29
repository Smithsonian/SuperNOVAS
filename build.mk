
# ============================================================================
# Generic build targets and recipes for SuperNOVAS.
# 
# You can include this in your Makefile also.
# ============================================================================


# Regular object files
$(OBJ)/%.o: $(SRC)/%.c $(OBJ) Makefile
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

# Share library recipe
$(LIB)/%.$(SOEXT).$(SO_VERSION): | $(LIB)
ifeq ($(UNAME_S),Darwin)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ -dynamiclib -fPIC -Wl,-install_name,@rpath/$(notdir $@) $(LDFLAGS)
else
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ -shared -fPIC -Wl,-soname,$(notdir $@) $(LDFLAGS)
endif

# Platform-dependent symbolic linkink
LNFLAGS := -sr
ifeq ($(UNAME_S),Darwin)
  LNFLAGS := -s
endif

# Unversioned shared libs (for linking against)
$(LIB)/lib%.$(SOEXT):
	@rm -f $@
	ln $(LNFLAGS) $< $@

# Static library recipe
$(LIB)/%.a:
	@$(MAKE) $(LIB)
	ar -rc $@ $^
	ranlib $@

# Create sub-directories for build targets
$(OBJ) $(LIB) $(BIN) apidoc:
	mkdir -p $@

# Remove intermediate files locally
.PHONY: clean-local
clean-local:
	rm -rf obj

# Remove all locally built files, effectively restoring the repo to its 
# pristine state
.PHONY: distclean-local
distclean-local: clean-local
	rm -rf bin lib apidoc infer-out

# Remove intermediate files (general)
.PHONY: clean
clean: clean-local

# Remove intermediate files (general)
.PHONY: distclean
distclean: distclean-local

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) $(SRC)

# Static code analysis viacat Facebook's infer
.PHONY: infer
infer: clean
	infer run -- $(MAKE) shared




