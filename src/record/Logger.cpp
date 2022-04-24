#include "sese/record/Logger.h"
#include "sese/Singleton.h"
#include "sese/record/ConsoleAppender.h"
#include "sese/record/SimpleFormatter.h"
#include <clocale>

namespace sese {

    void Logger::addAppender(const sese::AbstractAppender::Ptr &appender) noexcept {
        this->appenders.emplace_back(appender);
    }

    void Logger::log(const Event::Ptr &event) const noexcept {
        event->setLogName(this->name);
        for (auto &appender: appenders) {
            appender->preDump(event);
        }
    }

    Logger *getLogger() noexcept {
        return sese::Singleton<Logger>::getInstance();
    }

    int32_t LoggerInitiateTask::init() noexcept {
        // 初始化 Logger
        setlocale(LC_ALL, "");
        auto logger = sese::Singleton<Logger>::getInstance();
        auto formatter = std::make_shared<SimpleFormatter>();
        auto appender = std::make_shared<ConsoleAppender>(formatter);
        logger->addAppender(appender);
        return 0;
    }
}// namespace sese