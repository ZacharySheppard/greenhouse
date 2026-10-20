

class Device
{
public:
    Device() noexcept;
    ~Device();

    auto run() const -> void;
};