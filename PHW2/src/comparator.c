// TODO: write comparator initialization function
// - do not enable the module yet, just configure registers 
// - set up input muxes and setup reference voltage

// TODO write comparator interrupt
// - comparator will automatically trigger first ADC conversion by hardware so no need to do that
// - instead, here start the timer running to trigger the subsequent conversions
// - then disable the comparator and rising edge interrupt (bc trigger happens only once)

// TODO write comparator start function
// - this can be called when the Await trigger command is given
// - just enable the module here and enable the rising edge interrupt
