

PROTOS ?= $(wildcard *.proto)

BUILDDIR = build
BINPBS = $(patsubst %.proto,$(BUILDDIR)/%.binpb,$(PROTOS))

all: $(BINPBS)

$(BUILDDIR)/%.binpb: %.proto
	@mkdir -p $(@D)
	protoc -o $@ $<

clean:
	rm -rf $(BUILDDIR)
