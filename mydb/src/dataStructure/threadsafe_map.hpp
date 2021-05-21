#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <list>
#include <utility>
#include <shared_mutex>

// 将添加和改变操作整合进单个成员函数add_or_update_mapping()中
// 通过一定数量的桶（19）和哈希函数实现数据结构
// 每次操作只需要锁定一个桶
// 每个桶通过读写互斥元实现线程安全

/**
  * @brief          线程安全的map
  * @retval         none
  */
template<typename Key,typename Value,typename Hash=std::hash<Key> >
class threadsafe_map
{
private:
    /*
        bucket_type
    */
    typedef std::pair<Key,Value> bucket_value;
    typedef std::list<bucket_value> bucket_data;
    typedef typename bucket_data::iterator bucket_iterator;
    class bucket_type
    {
    private:

        bucket_data data;
        mutable std::shared_timed_mutex mutex;

        /*
            bucket_iterator find_entry_for() 查找key，返回key在list中的指针
            Value value_for() 通过std::shared_lock<std::shared_timed_mutex>读互斥元，调用find_entry_for查找key
            void add_or_update_mapping() 通过std::unique_lock<std::shared_timed_mutex>写互斥元，先查找元素，然后执行添加/更新键值对
            void remove_mapping() 通过std::unique_lock<std::shared_timed_mutex>写互斥元，先查找元素，然后执行删除
        */
        bucket_iterator find_entry_for(Key const& key)
        {
            return std::find_if(data.begin() , data.end() ,
                [&key](bucket_value const& item) // 比较key和item
                {return item.first==key;}); 
        }
        
    public:
        Value value_for(Key const& key,Value const& default_value) // 查询
        {
            std::shared_lock<std::shared_timed_mutex> lock(mutex);
            bucket_iterator found_entry=find_entry_for(key);
            return (found_entry==data.end())?
                default_value : found_entry->second;
        }

        void add_or_update_mapping(Key const& key,Value const& value) // 添加/更新
        {
            std::unique_lock<std::shared_timed_mutex> lock(mutex);
            bucket_iterator found_entry=find_entry_for(key);
            if(found_entry==data.end())
            {
                data.push_back(bucket_value(key,value));
            }
            else
            {
                found_entry->second=value;
            }
        }
    
        void remove_mapping(Key const& key) // 删除
        {
            std::unique_lock<std::shared_timed_mutex> lock(mutex);
            bucket_iterator found_entry=find_entry_for(key);
            if(found_entry!=data.end())
            {
                data.erase(found_entry);
            }
        }
    }; // bucket_type
    
    std::vector<std::unique_ptr<bucket_type> > buckets; // 桶，vector数组（每个元素是bucket的unique_ptr）
    Hash hasher; // 哈希函数

    bucket_type& get_bucket(Key const& key) // get_bucket 获取key应该在哪个bucket中，不需要锁
    {
        std::size_t const bucket_index=hasher(key)%buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;
    
    threadsafe_map(
        unsigned num_buckets=19, Hash const& hasher_=Hash()): // 构造函数 默认桶个数为19，哈希函数为Hash()
        buckets(num_buckets),hasher(hasher_)
    {
        for(unsigned i=0;i<num_buckets;++i)
        {
            buckets[i].reset(new bucket_type); // 通过std::make_unique或std::unique_ptr::reset初始化std::unique_ptr
        }
    }

    threadsafe_map(threadsafe_map const& other)=delete;
    threadsafe_map& operator=(
        threadsafe_map const& other)=delete;
    
    Value value_for(Key const& key, // 查找
        Value const& default_value=Value())
    {
        return get_bucket(key).value_for(key,default_value);
    }
    
    void add_or_update_mapping(Key const& key,Value const& value) // 插入/更新
    {
        get_bucket(key).add_or_update_mapping(key,value);
    }
    
    void remove_mapping(Key const& key) // 删除
    {
        get_bucket(key).remove_mapping(key);
    }
};

/**
  * @brief          扩展：本地化存储
  * @retval         none
  */
// std::map<Key,Value> threadsafe_map::get_map() const
// {
//     std::vector<std::unique_lock<boost::shared_mutex> > locks;
//     for(unsigned i=0;i<buckets.size();++i)
//     {
//         locks.push_back(
//             std::unique_lock<boost::shared_mutex>(buckets[i].mutex));
//     }
//     std::map<Key,Value> res;
//     for(unsigned i=0;i<buckets.size();++i)
//     {
//         for(bucket_iterator it=buckets[i].data.begin();
//             it!=buckets[i].data.end();
//             ++it)
//         {
//             res.insert(*it);
//         }
//     }
//     return res;
// }
