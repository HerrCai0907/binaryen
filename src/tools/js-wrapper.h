/*
 * Copyright 2017 WebAssembly Community Group participants
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Emit a JavaScript wrapper to run a wasm module with some test
// values, useful for fuzzing.
//

#include "wasm-type.h"
#include "wasm.h"
#include <string>

namespace wasm {

inline std::string generateJSWrapper(Module& wasm) {
  std::string ret;
  ret += "if (typeof console === 'undefined') {\n"
         "  console = { log: print };\n"
         "}\n"
         "var tempRet0;\n"
         "var binary;\n"
         "if (typeof process === 'object' && typeof require === 'function' /* "
         "node.js detection */) {\n"
         "  var args = process.argv.slice(2);\n"
         "  binary = require('fs').readFileSync(args[0]);\n"
         "  if (!binary.buffer) binary = new Uint8Array(binary);\n"
         "} else {\n"
         "  var args;\n"
         "  if (typeof scriptArgs != 'undefined') {\n"
         "    args = scriptArgs;\n"
         "  } else if (typeof arguments != 'undefined') {\n"
         "    args = arguments;\n"
         "  }\n"
         "  if (typeof readbuffer === 'function') {\n"
         "    binary = new Uint8Array(readbuffer(args[0]));\n"
         "  } else {\n"
         "    binary = read(args[0], 'binary');\n"
         "  }\n"
         "}\n"
         "function literal(x, type) {\n"
         "  var ret = '';\n"
         "  switch (type) {\n"
         "    case 'i32': ret += (x | 0); break;\n"
         "    case 'f32':\n"
         "    case 'f64': {\n"
         "      if (x == 0 && (1 / x) < 0) ret += '-';\n"
         "      ret += Number(x).toString();\n"
         "      break;\n"
         "    }\n"
         "    // For anything else, just print the type.\n"
         "    default: ret += type; break;\n"
         "  }\n"
         "  return ret;\n"
         "}\n"
         "var instance = new WebAssembly.Instance(new "
         "WebAssembly.Module(binary), {\n"
         "  'fuzzing-support': {\n"
         "    'log-i32': function(x)    { "
         "console.log('[LoggingExternalInterface logging ' + literal(x, 'i32') "
         "+ ']') },\n"
         "    'log-i64': function(x, y) { "
         "console.log('[LoggingExternalInterface logging ' + literal(x, 'i32') "
         "+ ' ' + literal(y, 'i32') + ']') },\n" // legalization: two i32s
         "    'log-f32': function(x)    { "
         "console.log('[LoggingExternalInterface logging ' + literal(x, 'f64') "
         "+ ']') },\n" // legalization: an f64
         "    'log-f64': function(x)    { "
         "console.log('[LoggingExternalInterface logging ' + literal(x, 'f64') "
         "+ ']') },\n"
         "  },\n"
         "  'env': {\n"
         "    'setTempRet0': function(x) { tempRet0 = x },\n"
         "    'getTempRet0': function() { return tempRet0 },\n"
         "  },\n"
         "});\n";
  for (auto& exp : wasm.exports) {
    if (exp->kind != ExternalKind::Function) {
      continue; // something exported other than a function
    }
    auto* func = wasm.getFunction(exp->value);
    ret += "try {\n";
    ret += std::string("  console.log('[fuzz-exec] calling ") +
           exp->name.toString() + "');\n";
    if (func->getResults() != Type::none) {
      ret += std::string("  console.log('[fuzz-exec] note result: ") +
             exp->name.toString() + " => ' + literal(";
    } else {
      ret += "  ";
    }
    ret += std::string("instance.exports.") + exp->name.toString() + "(";
    bool first = true;
    for (auto param : func->getParams()) {
      // zeros in arguments TODO more?
      if (first) {
        first = false;
      } else {
        ret += ", ";
      }
      if (param.isRef()) {
        ret += "null";
      } else {
        ret += "0";
        if (param == Type::i64) {
          ret += ", 0";
        }
      }
    }
    ret += ")";
    if (func->getResults() != Type::none) {
      ret += ", '" + func->getResults().toString() + "'))";
      // TODO: getTempRet
    }
    ret += ";\n";
    ret += "} catch (e) {\n";
    ret += "  console.log('exception!' /* + e */);\n";
    ret += "}\n";
  }
  return ret;
}

} // namespace wasm
