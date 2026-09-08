#pragma once

template <typename T>
class singleton
{
protected:
    singleton() = default;
    virtual ~singleton() = default;

public:
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    // shared_ptr 형태로 인스턴스를 반환합니다.
    static std::shared_ptr<T> instance()
    {
        static std::mutex mtx;
        std::lock_guard<std::mutex> lock(mtx);

        // weak_ptr에서 살이있는 shared_ptr를 얻으려고 시도
        std::shared_ptr<T> instance = instance_.lock();

        // 참조가 끊어져 파괴되었거나, 최초 호출 시 새로 생성
        if (!instance)
        {
            // struct Key 패턴 또는 make_shared 사용을 위해 커스텀 Deleter/Passkey 사용
            instance = std::shared_ptr<T>(new T());
            instance_ = instance; // weak_ptr에 약한 참조로 보관
        }

        return instance;
    }

private:
    // 객체의 생명주기를 직접 소유하지 않는 weak_ptr 정적 변수
    static inline std::weak_ptr<T> instance_{};
};