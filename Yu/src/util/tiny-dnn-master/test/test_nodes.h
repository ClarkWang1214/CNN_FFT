/*
    Copyright (c) 2016, Taiga Nomi
    All rights reserved.

    Use of this source code is governed by a BSD-style license that can be found
    in the LICENSE file.
*/
#pragma once
#include "gtest/gtest.h"
#include "testhelper.h"
#include "tiny_dnn/tiny_dnn.h"

using namespace tiny_dnn::layers;

namespace tiny_dnn {

TEST(nodes, sequential) {
  network<sequential> nn;

  nn << fc<tan_h>(10, 100) << fc<softmax>(100, 10);
}

TEST(nodes, graph_no_branch) {
  // declare nodes
  auto in = std::make_shared<input_layer>(shape3d(8, 8, 1));

  auto cnn = std::make_shared<convolutional_layer<tan_h>>(8, 8, 3, 1, 4);

  auto pool = std::make_shared<average_pooling_layer<tan_h>>(6, 6, 4, 2);

  auto out = std::make_shared<linear_layer<relu>>(3 * 3 * 4);

  // connect
  in << cnn << pool << out;

  network<graph> net;
  construct_graph(net, {in}, {out});
}

TEST(nodes, graph_branch) {
  // declare nodes
  auto in1   = std::make_shared<input_layer>(shape3d(3, 1, 1));
  auto in2   = std::make_shared<input_layer>(shape3d(3, 1, 1));
  auto added = std::make_shared<add>(2, 3);
  auto out   = std::make_shared<linear_layer<relu>>(3);

  // connect
  (in1, in2) << added;
  added << out;

  network<graph> net;
  construct_graph(net, {in1, in2}, {out});

  auto res = net.predict({{2, 4, 3}, {-1, 2, -5}})[0];

  // relu({2,4,3} + {-1,2,-5}) = {1,6,0}
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[0]), static_cast<float_t>(1.0));
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[1]), static_cast<float_t>(6.0));
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[2]), static_cast<float_t>(0.0));
}

TEST(nodes, graph_branch2) {
  // declare nodes
  input_layer in1(shape3d(3, 1, 1));
  input_layer in2(shape3d(3, 1, 1));
  add added(2, 3);
  linear_layer<relu> out(3);

  // connect
  (in1, in2) << added;
  added << out;

  network<graph> net;
  construct_graph(net, {&in1, &in2}, {&out});

  auto res = net.predict({{2, 4, 3}, {-1, 2, -5}})[0];

  // relu({2,4,3} + {-1,2,-5}) = {1,6,0}
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[0]), static_cast<float_t>(1.0));
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[1]), static_cast<float_t>(6.0));
  EXPECT_FLOAT_EQ(static_cast<float_t>(res[2]), static_cast<float_t>(0.0));
}

}  // namespace tiny-dnn