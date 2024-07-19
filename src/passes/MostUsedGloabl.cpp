/*
 * Copyright 2016 WebAssembly Community Group participants
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
// Removes obviously unneeded code
//

#include <ir/block-utils.h>
#include <ir/drop.h>
#include <ir/effects.h>
#include <ir/iteration.h>
#include <ir/literal-utils.h>
#include <ir/utils.h>
#include <map>
#include <memory>
#include <pass.h>
#include <wasm-builder.h>
#include <wasm.h>

namespace wasm {

struct MostUsedGlobalAnalyzer : public Pass {
  bool modifiesBinaryenIR() override { return false; }

  void run(Module* module) override {
    counter_.clear();

    for (std::unique_ptr<Global> const& global : module->globals) {
      counter_.insert_or_assign(global->name, 0U);
    }

    struct Counter : public PostWalker<Counter> {
      Counter(std::map<Name, uint32_t>& counter) : counter_(counter) {}

      void visitGlobalGet(GlobalGet* expr) { counter_[expr->name]++; }
      void visitGlobalSet(GlobalSet* expr) { counter_[expr->name]++; }

    private:
      std::map<Name, uint32_t>& counter_;
    };
    Counter{counter_}.doWalkModule(module);

    module->mostUsedGlobal = std::make_unique<MostUsedGlobal>();

    uint32_t maxCount = 0U;
    Name maxGlobalName;
    for (auto& [name, count] : counter_) {
      if (count >= maxCount) {
        maxCount = count;
        maxGlobalName = name;
      }
    }
    for (size_t i = 0; i < module->globals.size(); i++) {
      if (maxGlobalName == module->globals[i]->name) {
        module->mostUsedGlobal->globalIndex = i;
        break;
      }
    }
  }

private:
  std::map<Name, uint32_t> counter_;
};

Pass* createMostUsedGlobalPass() { return new MostUsedGlobalAnalyzer(); }

} // namespace wasm
