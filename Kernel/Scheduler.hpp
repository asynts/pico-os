#pragma once

namespace Kernel {
    class Thread {
    public:
        Thread(PageRange bss_segment, PageRange data_segment, PageRange text_segment);

        template<typename Callback>
        static Thread create(Callback callback);

        size_t id() const;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();

        void add_thread(Thread);

        void on_scheduler_timeout(Badge<InterruptManager>);
    };
}
