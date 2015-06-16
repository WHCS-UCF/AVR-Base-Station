/* needed to suppress error messages relating to this function
 * being missing. Tried to --gc-sections in the linker to no avail
 */
extern "C" void __cxa_pure_virtual() { while(1); }
