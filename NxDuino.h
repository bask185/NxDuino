enum NxDuinoStates {
    NxDuinoIDLE,
    getButtons,
    findAdjacentObject,
    storeNode,
    loadNode,
    fail,
    succes,
    store LED array,
    initNXduino };

extern bool NxDuino(void); 
extern void NxDuinoInit();
extern void NxDuinoSetState(unsigned char);
extern unsigned char NxDuinoGetState(void);
