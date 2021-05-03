enum NxDuinoStates {
    NxDuinoIDLE,
    getButtons,
    findAdjacentObject,
    storeNode,
    loadNode,
    fail,
    succes,
    storeLed,
    initNXduino };

extern bool NxDuino(void); 
extern void NxDuinoInit();
extern void NxDuinoSetState(unsigned char);
extern unsigned char NxDuinoGetState(void);
