#ifndef CCTC_TOX_H_
#define CCTC_TOX_H_

#include <memory>
#include <string>

namespace cctc {

std::string toxcore_version();

class Tox {
public:
    Tox();
    ~Tox();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cctc

#endif
