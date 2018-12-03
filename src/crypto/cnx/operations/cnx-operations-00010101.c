#include "crypto/cnx/operations/cnx-operations.h"

#include <inttypes.h>

void CN_ADAPTIVE_OPERATIONS_00010101(uint8_t* inPlaceOperand, const int8_t* appliedOperand, uint8_t op) {
  switch(op) {
    case 0:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 1:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 2:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 3:
      *inPlaceOperand = *appliedOperand;
      break;

    case 4:
      *inPlaceOperand += *appliedOperand;
      break;

    case 5:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 6:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 7:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 8:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 9:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 10:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 11:
      *inPlaceOperand += *appliedOperand;
      break;

    case 12:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 13:
      *inPlaceOperand = *appliedOperand;
      break;

    case 14:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 15:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 16:
      *inPlaceOperand += *appliedOperand;
      break;

    case 17:
      *inPlaceOperand = *appliedOperand;
      break;

    case 18:
      *inPlaceOperand = *appliedOperand;
      break;

    case 19:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 20:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 21:
      *inPlaceOperand += *appliedOperand;
      break;

    case 22:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 23:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 24:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 25:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 26:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 27:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 28:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 29:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 30:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 31:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 32:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 33:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 34:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 35:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 36:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 37:
      *inPlaceOperand = *appliedOperand;
      break;

    case 38:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 39:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 40:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 41:
      *inPlaceOperand += *appliedOperand;
      break;

    case 42:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 43:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 44:
      *inPlaceOperand += *appliedOperand;
      break;

    case 45:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 46:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 47:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 48:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 49:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 50:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 51:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 52:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 53:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 54:
      *inPlaceOperand += *appliedOperand;
      break;

    case 55:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 56:
      *inPlaceOperand = *appliedOperand;
      break;

    case 57:
      *inPlaceOperand = *appliedOperand;
      break;

    case 58:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 59:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 60:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 61:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 62:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 63:
      *inPlaceOperand = *appliedOperand;
      break;

    case 64:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 65:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 66:
      *inPlaceOperand = *appliedOperand;
      break;

    case 67:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 68:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 69:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 70:
      *inPlaceOperand = *appliedOperand;
      break;

    case 71:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 72:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 73:
      *inPlaceOperand += *appliedOperand;
      break;

    case 74:
      *inPlaceOperand += *appliedOperand;
      break;

    case 75:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 76:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 77:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 78:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 79:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 80:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 81:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 82:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 83:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 84:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 85:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 86:
      *inPlaceOperand = *appliedOperand;
      break;

    case 87:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 88:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 89:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 90:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 91:
      *inPlaceOperand = *appliedOperand;
      break;

    case 92:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 93:
      *inPlaceOperand += *appliedOperand;
      break;

    case 94:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 95:
      *inPlaceOperand += *appliedOperand;
      break;

    case 96:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 97:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 98:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 99:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 100:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 101:
      *inPlaceOperand = *appliedOperand;
      break;

    case 102:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 103:
      *inPlaceOperand += *appliedOperand;
      break;

    case 104:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 105:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 106:
      *inPlaceOperand = *appliedOperand;
      break;

    case 107:
      *inPlaceOperand = *appliedOperand;
      break;

    case 108:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 109:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 110:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 111:
      *inPlaceOperand = *appliedOperand;
      break;

    case 112:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 113:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 114:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 115:
      *inPlaceOperand = *appliedOperand;
      break;

    case 116:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 117:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 118:
      *inPlaceOperand += *appliedOperand;
      break;

    case 119:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 120:
      *inPlaceOperand = *appliedOperand;
      break;

    case 121:
      *inPlaceOperand += *appliedOperand;
      break;

    case 122:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 123:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 124:
      *inPlaceOperand += *appliedOperand;
      break;

    case 125:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 126:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 127:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 128:
      *inPlaceOperand += *appliedOperand;
      break;

    case 129:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 130:
      *inPlaceOperand = *appliedOperand;
      break;

    case 131:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 132:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 133:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 134:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 135:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 136:
      *inPlaceOperand = *appliedOperand;
      break;

    case 137:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 138:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 139:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 140:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 141:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 142:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 143:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 144:
      *inPlaceOperand += *appliedOperand;
      break;

    case 145:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 146:
      *inPlaceOperand = *appliedOperand;
      break;

    case 147:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 148:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 149:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 150:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 151:
      *inPlaceOperand = *appliedOperand;
      break;

    case 152:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 153:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 154:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 155:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 156:
      *inPlaceOperand = *appliedOperand;
      break;

    case 157:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 158:
      *inPlaceOperand = *appliedOperand;
      break;

    case 159:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 160:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 161:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 162:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 163:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 164:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 165:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 166:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 167:
      *inPlaceOperand = *appliedOperand;
      break;

    case 168:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 169:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 170:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 171:
      *inPlaceOperand += *appliedOperand;
      break;

    case 172:
      *inPlaceOperand += *appliedOperand;
      break;

    case 173:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 174:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 175:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 176:
      *inPlaceOperand = *appliedOperand;
      break;

    case 177:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 178:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 179:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 180:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 181:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 182:
      *inPlaceOperand += *appliedOperand;
      break;

    case 183:
      *inPlaceOperand = *appliedOperand;
      break;

    case 184:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 185:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 186:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 187:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 188:
      *inPlaceOperand += *appliedOperand;
      break;

    case 189:
      *inPlaceOperand += *appliedOperand;
      break;

    case 190:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 191:
      *inPlaceOperand = *appliedOperand;
      break;

    case 192:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 193:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 194:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 195:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 196:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 197:
      *inPlaceOperand += *appliedOperand;
      break;

    case 198:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 199:
      *inPlaceOperand += *appliedOperand;
      break;

    case 200:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 201:
      *inPlaceOperand += *appliedOperand;
      break;

    case 202:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 203:
      *inPlaceOperand = *appliedOperand;
      break;

    case 204:
      *inPlaceOperand = *appliedOperand;
      break;

    case 205:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 206:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 207:
      *inPlaceOperand += *appliedOperand;
      break;

    case 208:
      *inPlaceOperand = *appliedOperand;
      break;

    case 209:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 210:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 211:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 212:
      *inPlaceOperand += *appliedOperand;
      break;

    case 213:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 214:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 215:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 216:
      *inPlaceOperand = *appliedOperand;
      break;

    case 217:
      *inPlaceOperand += *appliedOperand;
      break;

    case 218:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 219:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 220:
      *inPlaceOperand = *appliedOperand;
      break;

    case 221:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 222:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 223:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 224:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 225:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 226:
      *inPlaceOperand += *appliedOperand;
      break;

    case 227:
      *inPlaceOperand += *appliedOperand;
      break;

    case 228:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 229:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 230:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 231:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 232:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 233:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 234:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 235:
      *inPlaceOperand = *appliedOperand;
      break;

    case 236:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 237:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 238:
      *inPlaceOperand = *appliedOperand;
      break;

    case 239:
      *inPlaceOperand = *appliedOperand;
      break;

    case 240:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 241:
      *inPlaceOperand = *appliedOperand;
      break;

    case 242:
      *inPlaceOperand = *appliedOperand;
      break;

    case 243:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 244:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 245:
      *inPlaceOperand += *appliedOperand;
      break;

    case 246:
      *inPlaceOperand ^= *appliedOperand;
      break;

    case 247:
      *inPlaceOperand = *appliedOperand;
      break;

    case 248:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 249:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 250:
      *inPlaceOperand -= *appliedOperand;
      break;

    case 251:
      *inPlaceOperand = ~*appliedOperand;
      break;

    case 252:
      *inPlaceOperand |= *appliedOperand;
      break;

    case 253:
      *inPlaceOperand = ~*inPlaceOperand;
      break;

    case 254:
      *inPlaceOperand &= *appliedOperand;
      break;

    case 255:
      *inPlaceOperand |= *appliedOperand;
      break;

    default:
        break;
  }
}
