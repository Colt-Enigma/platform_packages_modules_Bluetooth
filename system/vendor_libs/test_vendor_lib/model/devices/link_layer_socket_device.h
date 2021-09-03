/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stddef.h>  // for size_t

#include <cstdint>  // for uint8_t, uint32_t
#include <memory>   // for shared_ptr, make_shared
#include <string>   // for string
#include <vector>   // for vector

#include "device.h"                      // for Device
#include "include/phy.h"                 // for Phy, Phy::Type
#include "net/async_data_channel.h"      // for AsyncDataChannel
#include "packets/link_layer_packets.h"  // for LinkLayerPacketView

namespace test_vendor_lib {

using android::net::AsyncDataChannel;

class LinkLayerSocketDevice : public Device {
 public:
  LinkLayerSocketDevice(std::shared_ptr<AsyncDataChannel> socket_fd,
                        Phy::Type phy_type);
  LinkLayerSocketDevice(LinkLayerSocketDevice&& s) = default;
  virtual ~LinkLayerSocketDevice() = default;

  static std::shared_ptr<Device> Create(
      std::shared_ptr<AsyncDataChannel> socket_fd, Phy::Type phy_type) {
    return std::make_shared<LinkLayerSocketDevice>(socket_fd, phy_type);
  }

  virtual std::string GetTypeString() const override {
    return "link_layer_socket_device";
  }

  virtual void Initialize(const std::vector<std::string>&) override {}

  virtual void IncomingPacket(
      model::packets::LinkLayerPacketView packet) override;

  virtual void TimerTick() override;

  static constexpr size_t kSizeBytes = sizeof(uint32_t);

  void RegisterCloseCallback(std::function<void()>);

  void Close() override;

 private:
  std::shared_ptr<AsyncDataChannel> socket_;
  Phy::Type phy_type_;
  bool receiving_size_{true};
  size_t bytes_left_{kSizeBytes};
  size_t offset_{0};
  std::shared_ptr<std::vector<uint8_t>> size_bytes_;
  std::shared_ptr<std::vector<uint8_t>> received_;
  std::vector<model::packets::LinkLayerPacketView> packet_queue_;
};

}  // namespace test_vendor_lib
