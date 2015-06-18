#!/usr/bin/env python2
def emit_define(name, postfix, value):
    if name.find("#") != -1 and value.find("#") != -1:
        raise ValueError("Found processor metacharacters")

    return "#define " + name + postfix + " " + value + "\n"

f = open("pins.def", "r").read()
fOut = open("pins.def.h", "w")

for line in f.split("\n"):
    if not line.startswith("PINOUT"):
        fOut.write(line + "\n")
        continue

    line = line[line.find("(")+1:line.find(")")]
    args = line.split(',')
    args = [a.strip() for a in args]

    pinName = args[0]
    port = args[1]
    pinDef = args[2]

    fOut.write(emit_define(pinName, "_PORT", "PORT" + port))
    fOut.write(emit_define(pinName, "_DDR", " DDR" + port))
    fOut.write(emit_define(pinName, "_PIN", " PIN" + port))

    if pinDef.isdigit():
        pinNumber = int(pinDef)
        if pinNumber < 0 or pinNumber > 7:
            raise ValueError("invalid pin bit definition")

        fOut.write(emit_define(pinName, "_MSK", " _BV(%s)" % pinDef))
        fOut.write(emit_define(pinName, "_NUMBER", " %s" % pinDef))
    elif pinDef.startswith("0b"):
        pinNumber = int(pinDef[2:], 2) # parse binary value
        if pinNumber < 0 or pinNumber > 255:
            raise ValueError("invalid pin bit definition: %s" % pinDef)

        fOut.write(emit_define(pinName, "_MSK", " " + hex(pinNumber)))

    fOut.write("\n")
    #print args


