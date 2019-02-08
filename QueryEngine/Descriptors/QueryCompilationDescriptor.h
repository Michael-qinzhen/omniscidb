/*
 * Copyright 2019 OmniSci, Inc.
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

/**
 * @file    QueryCompilationDescriptor.h
 * @author  Alex Baden <alex.baden@mapd.com>
 * @brief   Container for compilation results and assorted options for a single execution
 * unit.
 */

#pragma once

#include "../Execute.h"

class QueryCompilationDescriptor {
 public:
  QueryCompilationDescriptor(const std::shared_ptr<RowSetMemoryOwner> row_set_mem_owner)
      : row_set_mem_owner_(row_set_mem_owner)
      , compilation_device_type_(ExecutorDeviceType::CPU)
      , hoist_literals_(false) {}

  int8_t compile(const size_t max_groups_buffer_entry_guess,
                 const int8_t crt_min_byte_width,
                 const bool has_cardinality_estimation,
                 const RelAlgExecutionUnit& ra_exe_unit,
                 const std::vector<InputTableInfo>& table_infos,
                 const CompilationOptions& co,
                 const ExecutionOptions& eo,
                 const Catalog_Namespace::Catalog& cat,
                 RenderInfo* render_info,
                 Executor::ExecutionDispatch* execution_dispatch,
                 Executor* executor);

  auto getCompilationResult() const { return compilation_result_; }

  std::string getIR() const {
    switch (compilation_device_type_) {
      case ExecutorDeviceType::CPU: {
        return std::string{"IR for the CPU:\n===============\n" +
                           compilation_result_.llvm_ir};
      }
      case ExecutorDeviceType::GPU: {
        return std::string{"IR for the GPU:\n===============\n" +
                           compilation_result_.llvm_ir};
      }
    }
    UNREACHABLE();
    return "";
  }

  const QueryMemoryDescriptor& getQueryMemoryDescriptor() {
    return compilation_result_.query_mem_desc;
  }

  ExecutorDeviceType getDeviceType() const { return compilation_device_type_; }
  bool hoistLiterals() const { return hoist_literals_; }

 private:
  const std::shared_ptr<RowSetMemoryOwner> row_set_mem_owner_;

  Executor::CompilationResult compilation_result_;
  ExecutorDeviceType compilation_device_type_;
  bool hoist_literals_;
};