/*
 * Copyright (c) 2017-2018 ARM Limited.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "arm_compute/runtime/TensorAllocator.h"

#include "arm_compute/core/utils/misc/Utility.h"
#include "arm_compute/runtime/MemoryGroup.h"
#include "arm_compute/runtime/MemoryRegion.h"

#include "support/ToolchainSupport.h"

#include "tests/Utils.h"
#include "tests/framework/Asserts.h"
#include "tests/framework/Macros.h"

namespace arm_compute
{
namespace test
{
namespace validation
{
TEST_SUITE(NEON)
TEST_SUITE(UNIT)
TEST_SUITE(TensorAllocator)

TEST_CASE(ImportMemory, framework::DatasetMode::ALL)
{
    // Init tensor info
    TensorInfo info(TensorShape(24U, 16U, 3U), 1, DataType::F32);

    // Allocate memory buffer
    const size_t total_size = info.total_size();
    auto         data       = support::cpp14::make_unique<uint8_t[]>(total_size);

    // Negative case : Import pointer with zero size
    Tensor t1;
    t1.allocator()->init(info);
    ARM_COMPUTE_EXPECT(!bool(t1.allocator()->import_memory(data.get(), 0)), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t1.info()->is_resizable(), framework::LogLevel::ERRORS);

    // Negative case : Import nullptr
    Tensor t2;
    t2.allocator()->init(info);
    ARM_COMPUTE_EXPECT(!bool(t2.allocator()->import_memory(nullptr, total_size)), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t2.info()->is_resizable(), framework::LogLevel::ERRORS);

    // Negative case : Import memory to a tensor that is memory managed
    Tensor      t3;
    MemoryGroup mg;
    t3.allocator()->set_associated_memory_group(&mg);
    ARM_COMPUTE_EXPECT(!bool(t3.allocator()->import_memory(data.get(), total_size)), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t3.info()->is_resizable(), framework::LogLevel::ERRORS);

    // Positive case : Set raw pointer
    Tensor t4;
    t4.allocator()->init(info);
    ARM_COMPUTE_EXPECT(bool(t4.allocator()->import_memory(data.get(), total_size)), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(!t4.info()->is_resizable(), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t4.buffer() == reinterpret_cast<uint8_t *>(data.get()), framework::LogLevel::ERRORS);
    t4.allocator()->free();
    ARM_COMPUTE_EXPECT(t4.info()->is_resizable(), framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t4.buffer() == nullptr, framework::LogLevel::ERRORS);
}

TEST_CASE(AlignedAlloc, framework::DatasetMode::ALL)
{
    // Init tensor info
    TensorInfo   info(TensorShape(24U, 16U, 3U), 1, DataType::F32);
    const size_t requested_alignment = 1024;

    Tensor t;
    t.allocator()->init(info, requested_alignment);
    t.allocator()->allocate();

    ARM_COMPUTE_EXPECT(t.buffer() != nullptr, framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(t.allocator()->alignment() == requested_alignment, framework::LogLevel::ERRORS);
    ARM_COMPUTE_EXPECT(arm_compute::utility::check_aligned(reinterpret_cast<void *>(t.buffer()), requested_alignment),
                       framework::LogLevel::ERRORS);
}

TEST_SUITE_END()
TEST_SUITE_END()
TEST_SUITE_END()
} // namespace validation
} // namespace test
} // namespace arm_compute
