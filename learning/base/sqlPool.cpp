#include <learning/base/sqlPool.h>
#include <learning/base/logging.h>

using namespace learning;

sqlPool::sqlPool(const std::string& name)
  : name_(name),
    maxPoolSize_(0),
    quited_(false),
    threadInitCallback_(nullptr),
    useSelect_("dbname=wlle user=postgres password=333333 \
      hostaddr=127.0.0.1 port=5432")
{}

sqlPool::~sqlPool()
{
    stop();
}

void sqlPool::start()
{
    LOG_DEBUG << "sqlPool start";
    for(size_t i = 0; i != maxPoolSize_; ++i) {
        auto t = std::make_unique<thread>(std::bind(&sqlPool::_runInThread, this, i));
        t->start();
        pool_.push_back(move(t));
    }
    if(maxPoolSize_ == 0 && threadInitCallback_) {
        threadInitCallback_();
    }
}
void sqlPool::stop()
{
    quited_ = true;
    notEmpty_.notify_all();
}

void sqlPool::_runInThread(int i)
    {
        if(threadInitCallback_) {
            threadInitCallback_();
        }

        pqxx::connection c(
            // 后续可以放在json里
            "dbname=wlle user=postgres password=333333 \
      hostaddr=127.0.0.1 port=5432");
        if (!c.is_open()) {
            LOG_ERROR << "打开sql连接失败";
            return;
        } else {
            LOG_ERROR << "线程" << i << "打开sql成功";
        }

        std::unique_lock<std::mutex> lock(mutex_);
        while(!quited_) {
            while(tasks_.empty() && !quited_) {
                notEmpty_.wait(lock);
            }
            if(!tasks_.empty() && !quited_) {
                threadTask it = tasks_.front();
                LOG_DEBUG << "执行task";
                tasks_.pop_front();
                try {
                    it(&c);
                } catch(pqxx::sql_error& x) { // 当执行 SQL 查询时发生错误，例如语法错误、表不存在等
                    LOG_ERROR << "op error: " << x.what();
                } catch(pqxx::broken_connection& x) { // 当与数据库的连接断开或中断时引发的错误
                    LOG_ERROR << "op error: " << x.what();
                } catch(pqxx::transaction_rollback& x) { // 当 pqxx 库内部出现问题或调用失败时引发的错误
                    LOG_ERROR << "op error: " << x.what();
                } catch(pqxx::failure& x) { // 当事务回滚时引发的错误，表示事务中的某些操作失败
                    LOG_ERROR << "op error: " << x.what();
                } catch(pqxx::usage_error& x) { // 当使用 pqxx 库的不正确方式时引发的错误
                    LOG_ERROR << "op error: " << x.what();
                } catch(pqxx::internal_error& x) { // 当 pqxx 库内部发生意外错误时引发的错误
                    LOG_ERROR << "op error: " << x.what();
                } catch(...) {
                    LOG_ERROR << "未知错误";
                }
            }
        }
        c.disconnect();
    }