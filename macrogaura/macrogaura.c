/**
 * macrogaura
 * Copyright © 2019 Le Bao Hiep. All rights reserved.
 *
 * Author:        Le Bao Hiep <baohiep2013@gmail.com>
 * Creation Date: 20 July 2019
 *
 * Description:
 *    RGB keyboard control for Asus ROG laptops
 *
 * Revision Information:
 *
 *    20 July 2019
 *       - First version
 *
 * \file macrogaura.c
 */

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/hid/IOHIDLib.h>

#define MESSAGE_LENGTH 17
#define MAX_NUM_MESSAGES 6
#define MAX_NUM_COLORS 4
#define MAX_FUNCNAME_LEN 32

// verbose output
int verbose = 0;
#define V(x) if (verbose) { x; }

// ------------------------------------------------------------
//  Data structures
// ------------------------------------------------------------

typedef struct {
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
} Color;

typedef enum {Slow = 1, Medium, Fast} Speed;

typedef struct {
    Color colors[MAX_NUM_COLORS];
    Speed speed;
} Arguments;

typedef struct {
    int nMessages;
    uint8_t messages[MAX_NUM_MESSAGES][MESSAGE_LENGTH];
} Messages;

typedef struct {
    const char *szName;
    void (*function)(Arguments *args, Messages *outputs);
    int nColors;
    int nSpeed;
} FunctionRecord;

// ------------------------------------------------------------
//  USB protocol for RGB keyboard
// ------------------------------------------------------------

const uint8_t SPEED_BYTE_VALUES[] = {0xe1, 0xeb, 0xf5};

uint8_t speedByteValue(Speed speed) {
    return SPEED_BYTE_VALUES[speed - 1];
}

uint8_t MESSAGE_SET[] = {0x5d, 0xb5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t MESSAGE_APPLY[] = {0x5d, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void initMessage(uint8_t *msg) {
    memset(msg, 0, MESSAGE_LENGTH);
    msg[0] = 0x5d;
    msg[1] = 0xb3;
}

void single_static(Arguments *args, Messages *outputs) {
    V(printf("single_static\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[4] = args->colors[0].nRed;
    m[5] = args->colors[0].nGreen;
    m[6] = args->colors[0].nBlue;
}

void single_breathing(Arguments *args, Messages *outputs) {
    V(printf("single_breathing\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[3] = 1;
    m[4] = args->colors[0].nRed;
    m[5] = args->colors[0].nGreen;
    m[6] = args->colors[0].nBlue;
    m[7] = speedByteValue(args->speed);
    m[9] = 1;
    m[10] = args->colors[1].nRed;
    m[11] = args->colors[1].nGreen;
    m[12] = args->colors[1].nBlue;
}

void single_colorcycle(Arguments *args, Messages *outputs) {
    V(printf("single_colorcycle\n"));
    outputs->nMessages = 1;
    uint8_t *m = outputs->messages[0];
    initMessage(m);
    m[3] = 2;
    m[4] = 0xff;
    m[7] = speedByteValue(args->speed);
}

void multi_static(Arguments *args, Messages *outputs) {
    V(printf("multi_static\n"));
    outputs->nMessages = 4;
    for (int i = 0; i < 4; ++i) {
        uint8_t *m = outputs->messages[i];
        initMessage(m);
        m[2] = i + 1;
        m[4] = args->colors[i].nRed;
        m[5] = args->colors[i].nGreen;
        m[6] = args->colors[i].nBlue;
        m[7] = 0xeb;
    }
}

void multi_breathing(Arguments *args, Messages *outputs) {
    V(printf("multi_breathing\n"));
    outputs->nMessages = 4;
    for (int i = 0; i < 4; ++i) {
        uint8_t *m = outputs->messages[i];
        initMessage(m);
        m[2] = i + 1;
        m[3] = 1;
        m[4] = args->colors[i].nRed;
        m[5] = args->colors[i].nGreen;
        m[6] = args->colors[i].nBlue;
        m[7] = speedByteValue(args->speed);
    }
}

const uint8_t RED[] = { 0xff, 0x00, 0x00 };
const uint8_t GREEN[] = { 0x00, 0xff, 0x00 };
const uint8_t BLUE[] = { 0x00, 0x00, 0xff };
const uint8_t YELLOW[] = { 0xff, 0xff, 0x00 };
const uint8_t CYAN[] = { 0x00, 0xff, 0xff };
const uint8_t MAGENTA[] = { 0xff, 0x00, 0xff };
const uint8_t WHITE[] = { 0xff, 0xff, 0xff };
const uint8_t BLACK[] = { 0x00, 0x00, 0x00 };

void red(Arguments *args, Messages *messages) {
    memcpy(args->colors, RED, 3);
    single_static(args, messages);
}

void green(Arguments *args, Messages *messages) {
    memcpy(args->colors, GREEN, 3);
    single_static(args, messages);
}

void blue(Arguments *args, Messages *messages) {
    memcpy(args->colors, BLUE, 3);
    single_static(args, messages);
}

void yellow(Arguments *args, Messages *messages) {
    memcpy(args->colors, YELLOW, 3);
    single_static(args, messages);
}

void cyan(Arguments *args, Messages *messages) {
    memcpy(args->colors, CYAN, 3);
    single_static(args, messages);
}

void magenta(Arguments *args, Messages *messages) {
    memcpy(args->colors, MAGENTA, 3);
    single_static(args, messages);
}

void white(Arguments *args, Messages *messages) {
    memcpy(args->colors, WHITE, 3);
    single_static(args, messages);
}

void black(Arguments *args, Messages *messages) {
    memcpy(args->colors, BLACK, 3);
    single_static(args, messages);
}

void rainbow(Arguments *args, Messages *messages) {
    memcpy(&(args->colors[0]), RED, 3);
    memcpy(&(args->colors[1]), YELLOW, 3);
    memcpy(&(args->colors[2]), CYAN, 3);
    memcpy(&(args->colors[3]), MAGENTA, 3);
    multi_static(args, messages);
}

// ------------------------------------------------------------
//  Command line argument parsing
// ------------------------------------------------------------

const FunctionRecord FUNCTION_RECORDS[] = {
    {"single_static", &single_static, 1, 0},
    {"single_breathing", &single_breathing, 2, 1},
    {"single_colorcycle", &single_colorcycle, 0, 1},
    {"multi_static", &multi_static, 4, 0},
    {"multi_breathing", &multi_breathing, 4, 1},
    {"red", &red, 0, 0},
    {"green", &green, 0, 0},
    {"blue", &blue, 0, 0},
    {"yellow", &yellow, 0, 0},
    {"cyan", &cyan, 0, 0},
    {"magenta", &magenta, 0, 0},
    {"white", &white, 0, 0},
    {"black", &black, 0, 0},
    {"rainbow", &rainbow, 0, 0},
};

const int NUM_FUNCTION_RECORDS = (int)(sizeof(FUNCTION_RECORDS) / sizeof(FUNCTION_RECORDS[0]));

void usage() {
    printf("macrogaura - RGB keyboard control for Asus ROG laptops\n");
#ifdef MODULE_VERSION
#define xStringify(a) Stringify(a)
#define Stringify(a) #a
    printf("Version %s\n\n", xStringify(MODULE_VERSION));
#endif
    printf("Copyright © 2019 Le Bao Hiep\n\n");
    printf("Usage:\n");
    printf("   macrogaura COMMAND ARGUMENTS\n\n");
    printf("COMMAND should be one of:\n");
    for (int i = 0; i < NUM_FUNCTION_RECORDS; ++i) {
        printf("   %s\n", FUNCTION_RECORDS[i].szName);
    }
}

int parseColor(char *arg, Color *pResult) {
    V(printf("parse color %s\n", arg));
    uint32_t v = 0;
    if (strlen(arg) != 6) goto fail;
    for (int i = 0; i < 6; ++i) {
        if (!isxdigit(arg[i])) goto fail;
    }
    v = (uint32_t)strtol(arg, 0, 16);
    if (errno == ERANGE) goto fail;
    pResult->nRed = (v >> 16) & 0xff;
    pResult->nGreen = (v >> 8) & 0xff;
    pResult->nBlue = v & 0xff;
    V(printf("interpreted color %d %d %d\n", pResult->nRed, pResult->nGreen, pResult->nBlue));
    return 0;
fail:
    fprintf(stderr, "Could not interpret color parameter value %s\n", arg);
    fprintf(stderr, "Please give this value as a six-character hex string like ff0000.\n");
    return -1;
}

int parseSpeed(char *arg, Speed *pResult) {
    V(printf("parse speed %s\n", arg));
    long nSpeed = strtol(arg, 0, 0);
    if (errno == ERANGE || nSpeed < 1 || nSpeed > 3) {
        fprintf(stderr, "Could not interpret speed parameter value %s\n", arg);
        fprintf(stderr, "Please give this value as an integer: 1 (slow), 2 (medium), or 3 (fast).\n");
        return -1;
    }
    *pResult = (Speed)nSpeed;
    return 0;
}

int parseArguments(int argc, char **argv, Messages *messages) {
    int                   nRetval;
    Arguments             args;
    int                   nArgs         = 0;
    const FunctionRecord *pDesiredFunc  = 0;
    int                   nColors       = 0;
    
    // check for command line options
    while ((nRetval = getopt(argc, argv, "v")) != -1) {
        switch (nRetval) {
            case 'v':
                verbose = 1;
                break;
            default: /* '?' */
                usage();
                return -1;
        }
    }
    nArgs = argc - optind;
    
    // identify the function the user has asked for
    if (nArgs > 0) {
        for (int i = 0; i < NUM_FUNCTION_RECORDS; ++i) {
            if (!strncmp(argv[optind], FUNCTION_RECORDS[i].szName, MAX_FUNCNAME_LEN)) {
                pDesiredFunc = &(FUNCTION_RECORDS[i]);
                break;
            }
        }
    }
    if (!pDesiredFunc) {
        usage();
        return -1;
    }
    // check that the function signature is satisfied
    if (nArgs != (1 + pDesiredFunc->nColors + pDesiredFunc->nSpeed)) {
        usage();
        printf("\nFunction %s takes ", pDesiredFunc->szName);
        if (pDesiredFunc->nColors > 0) {
            if (pDesiredFunc->nSpeed) {
                printf("%d color(s) and a speed", pDesiredFunc->nColors);
            } else {
                printf("%d color(s)", pDesiredFunc->nColors);
            }
        } else {
            if (pDesiredFunc->nSpeed) {
                printf("a speed");
            } else {
                printf("no arguments");
            }
        }
        printf(":\n   macrogaura %s ", pDesiredFunc->szName);
        for (int i = 0; i < pDesiredFunc->nColors; i++) {
            printf("COLOR%d ", i+1);
        }
        if (pDesiredFunc->nSpeed) {
            printf("SPEED");
        }
        printf("\n\nCOLOR argument(s) should be given as hex values like ff0000\n");
        printf("SPEED argument should be given as an integer: 1, 2, or 3\n");
        return -1;
    }
    // parse the argument values
    for (int i = optind + 1; i < argc; ++i) {
        if (nColors < pDesiredFunc->nColors) {
            nRetval = parseColor(argv[i], &(args.colors[nColors]));
            if (nRetval < 0) return -1;
            nColors++;
        } else {
            nRetval = parseSpeed(argv[i], &args.speed);
            if (nRetval < 0) return -1;
        }
    }
    V(printf("args:\n"));
    for (int i = 0; i < MAX_NUM_COLORS; ++i) {
        V(printf("color%d %d %d %d\n", i + 1, args.colors[i].nRed, args.colors[i].nGreen, args.colors[i].nBlue));
    }
    V(printf("speed %d\n", args.speed));
    // call the function the user wants
    pDesiredFunc->function(&args, messages);
    V(printf("constructed %d messages:\n", messages->nMessages));
    for (int i = 0; i < messages->nMessages; ++i) {
        V(printf("message %d: ", i));
        for (int j = 0; j < MESSAGE_LENGTH; j++)
        {
            V(printf("%02x ", messages->messages[i][j]));
        }
        V(printf("\n"));
    }
    return 0;
}

// ------------------------------------------------------------
//  IOHIDDevice interface
// ------------------------------------------------------------

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key) {
    CFTypeRef ref;
    int32_t value;
    
    ref = IOHIDDeviceGetProperty(device, key);
    if (ref) {
        if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
            CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
            return value;
        }
    }
    return 0;
}

static int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len) {
    CFStringRef str;
    
    if (!len)
        return 0;
    
    str = (CFStringRef) IOHIDDeviceGetProperty(device, prop);
    
    buf[0] = 0;
    
    if (str) {
        CFIndex str_len = CFStringGetLength(str);
        CFRange range;
        CFIndex used_buf_len;
        CFIndex chars_copied;
        
        len --;
        
        range.location = 0;
        range.length = ((size_t) str_len > len)? len: (size_t) str_len;
        chars_copied = CFStringGetBytes(str,
                                        range,
                                        kCFStringEncodingUTF32LE,
                                        (char) '?',
                                        FALSE,
                                        (UInt8*)buf,
                                        len * sizeof(wchar_t),
                                        &used_buf_len);
        
        if (chars_copied == len)
            buf[len] = 0; /* len is decremented above */
        else
            buf[chars_copied] = 0;
        
        return 0;
    }
    else
        return -1;
    
}

const uint16_t ASUS_VENDOR_ID = 0x0b05;
const uint16_t ASUS_PRODUCT_IDS[] = { 0x1854, 0x1869, 0x1866 };
const int NUM_ASUS_PRODUCTS = (int)(sizeof(ASUS_PRODUCT_IDS) / sizeof(ASUS_PRODUCT_IDS[0]));

int handleUsb(Messages *pMessages) {
    IOHIDManagerRef tIOHIDManagerRef = NULL;
    IOHIDDeviceRef *tIOHIDDeviceRefs = NULL;
    CFSetRef deviceCFSetRef= NULL;
    
    // create a IO HID Manager reference
    tIOHIDManagerRef = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!tIOHIDManagerRef) goto exit;
    
    // set the HID device matching dictionary
    IOHIDManagerSetDeviceMatching(tIOHIDManagerRef, NULL);
    
    // Now open the IO HID Manager reference
    IOReturn tIOReturn = IOHIDManagerOpen(tIOHIDManagerRef, kIOHIDOptionsTypeNone);
    if (tIOReturn == kIOReturnError) goto exit;
    
    // and copy out its devices
    deviceCFSetRef = IOHIDManagerCopyDevices(tIOHIDManagerRef);
    if (!deviceCFSetRef) goto exit;
    
    // how many devices in the set?
    CFIndex deviceCount = CFSetGetCount(deviceCFSetRef);
    
    // convert the list into a C array so we can iterate easily
    tIOHIDDeviceRefs = calloc(deviceCount, sizeof(IOHIDDeviceRef));
    CFSetGetValues(deviceCFSetRef, (const void **) tIOHIDDeviceRefs);
    
    for (int i = 0; i < deviceCount; i++) {
        IOHIDDeviceRef dev = tIOHIDDeviceRefs[i];
        int dev_vid = get_int_property(dev, CFSTR(kIOHIDVendorIDKey));
        int dev_pid = get_int_property(dev, CFSTR(kIOHIDProductIDKey));
        if (dev_vid == ASUS_VENDOR_ID) {
            for (int i = 0; i < NUM_ASUS_PRODUCTS; ++i)
            {
                if (dev_pid == ASUS_PRODUCT_IDS[i]) {
                    wchar_t buf[256];
                    get_string_property(dev, CFSTR(kIOHIDProductKey), buf, 256);
                    printf("Found ROG Aura keyboard: %p\n", dev);
                    printf("  VendorID: %04x\n", dev_vid);
                    printf("  ProductID: %04x\n", dev_pid);
                    printf("  Product: %ls\n", buf);
                    // Send the messages
                    for (int i = 0; i < pMessages->nMessages; ++i) {
                        IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, pMessages->messages[i][0], pMessages->messages[i], MESSAGE_LENGTH);
                    }
                    IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, MESSAGE_SET[0], MESSAGE_SET, MESSAGE_LENGTH);
                    IOHIDDeviceSetReport(dev, kIOHIDReportTypeFeature, MESSAGE_APPLY[0], MESSAGE_APPLY, MESSAGE_LENGTH);
                }
            }
        }
    }
    
exit:
    if (deviceCFSetRef) CFRelease(deviceCFSetRef);
    if (tIOHIDDeviceRefs) free(tIOHIDDeviceRefs);
    if (tIOHIDManagerRef) CFRelease(tIOHIDManagerRef);
    return 0;
}

// ------------------------------------------------------------
//  Main function
// ------------------------------------------------------------

int main(int argc, char **argv) {
    Messages messages;
    if (parseArguments(argc, argv, &messages) == 0) {
        handleUsb(&messages);
    }
}
