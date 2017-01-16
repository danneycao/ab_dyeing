#include <cstring>
#include <cassert>
#include <signal.h>
#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>

#include "kafka_consumer.h"

KafkaConsumer::KafkaConsumer(const std::vector<std::pair<std::string, uint16_t> >& brokers,
        const std::string& group, const std::string& topic)
    :inited_(false), brokers_(brokers), group_(group), topic_(topic)
{
}

KafkaConsumer::~KafkaConsumer()
{
    rd_kafka_consumer_close(this->rk_);
    rd_kafka_topic_partition_list_destroy(this->topics_);
    rd_kafka_destroy(this->rk_);
    int run = 5;
    while (run-- > 0 && rd_kafka_wait_destroyed(1000) == -1);
}

int32_t KafkaConsumer::Init()
{
    this->inited_ = false;
    this->conf_ = rd_kafka_conf_new();
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(this->conf_, "internal.termination.signal", tmp, NULL, 0);
    //rd_kafka_conf_set(this->conf_, "auto.offset.reset", "earliest", NULL, 0);
    rd_kafka_conf_set(this->conf_, "auto.offset.reset", "smallest", NULL, 0); //必须要加，如果要读旧数据

    this->topic_conf_ = rd_kafka_topic_conf_new();

    char errstr[512];
    if (rd_kafka_conf_set(this->conf_, "group.id", this->group_.c_str(),
                errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) 
    {
        this->errmsg_ = errstr;
        return -1;
    }
    if (rd_kafka_topic_conf_set(this->topic_conf_, "offset.store.method",
                "broker", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
    {
        this->errmsg_ = errstr;
        return -1;
    }

    /* Set default topic config for pattern-matched topics. */
    rd_kafka_conf_set_default_topic_conf(this->conf_, this->topic_conf_);

    /* Callback called on partition assignment changes */
    rd_kafka_conf_set_rebalance_cb(this->conf_, KafkaConsumer::RebalanceCb);

    // rd_kafka_conf_set_log_cb(conf, logger);
    if (!(this->rk_ = rd_kafka_new(RD_KAFKA_CONSUMER, this->conf_,
                    errstr, sizeof(errstr)))) 
    {
        this->errmsg_ = errstr;
        return -1;
    }
    // rd_kafka_set_log_level(this->rk_, LOG_DEBUG);

    std::string brokers;
    for (std::vector<std::pair<std::string, uint16_t> >::const_iterator it = this->brokers_.begin(); it != this->brokers_.end(); ++it)
    {
        if (it != this->brokers_.begin())
        {
            brokers += ",";
        }
        brokers += it->first + ":" + boost::lexical_cast<std::string>(it->second);
    }
    if (rd_kafka_brokers_add(this->rk_, brokers.c_str()) == 0)
    {
        this->errmsg_ = "No valid brokers specified";
        return -1;
    }

    //can listen multiple topics
    this->topics_ = rd_kafka_topic_partition_list_new(1);
    
    std::ifstream fin("offset.txt");
    std::string str_p_max;
    getline(fin, str_p_max);
    if (str_p_max.empty()) {
    
        rd_kafka_topic_partition_list_add(this->topics_, this->topic_.c_str(), -1);
    } else {

        int64_t p_max = std::atoi(str_p_max.c_str()); 
        for (auto p_size = 0; p_size <= p_max; p_size++) {

            std::string file_name = "offset.partition" + p_size;
            std::ifstream fins(file_name);
            std::string p_offset;
            getline(fins, p_offset);
            
            int64_t reset_offset = std::atoi(p_offset.c_str()); 
            rd_kafka_topic_partition_list_add(this->topics_, this->topic_.c_str(), p_size)->offset = reset_offset;
            fins.close();
        }
        rd_kafka_assign(this->rk_, this->topics_);
    }
    fin.close();

    if ((this->err_ = rd_kafka_subscribe(this->rk_, this->topics_)))
    {
        this->errmsg_ = std::string("Failed to start consuming") + rd_kafka_err2str(this->err_);
        return -1;
    }

    this->inited_ = true;
    return 0;
}

int32_t KafkaConsumer::FetchMsg(std::string& msg)
{
    msg.clear();
    if (!this->inited_)
    {
        return -1;
    }

    rd_kafka_message_t* rkmessage = rd_kafka_consumer_poll(this->rk_, 1000);
    if (!rkmessage)
    {
        this->errmsg_ = "No message";
        return 1;
    }

    if (rkmessage->err) 
    {
        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
                rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
        {
            assert(0);
            rd_kafka_message_destroy(rkmessage);
            return -1;
        }
        if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
        {
            this->errmsg_ = "Consumer reached end of message queue partition: " 
                + std::to_string((long long int)rkmessage->partition) + 
                "at offset: " + boost::lexical_cast<std::string>(rkmessage->offset);
            rd_kafka_message_destroy(rkmessage);
            return 1;
        }

        this->errmsg_ = rd_kafka_message_errstr(rkmessage);
        rd_kafka_message_destroy(rkmessage);
        return -1;
    }
    {
      	time_t timep;
    	struct tm *p_tm;
    	timep = time(NULL);
    	p_tm = localtime(&timep);

        static int max_partition = 0;
        if (1== p_tm->tm_hour && p_tm->tm_min < 5) {

            std::string pre_file_name = "offset.partition";
            std::string file_name =  pre_file_name + std::to_string((long long int)rkmessage->partition);
            std::ofstream fout(file_name, std::ofstream::out | std::ofstream::ate);
            fout << rkmessage->offset;
            fout.close();

            if (rkmessage->partition > max_partition) {
                max_partition = rkmessage->partition;
                
                std::ofstream fouts("offset.txt", std::ofstream::out | std::ofstream::ate);
                fouts << max_partition;
                fouts.close();
            }
            //printf("the_file_name (%s) partition (%d) offset (%ld) \n", file_name.c_str(), rkmessage->partition, rkmessage->offset);
        }
    }
    msg = std::string((char *)rkmessage->payload, rkmessage->len);
    rd_kafka_message_destroy(rkmessage);
    return 0;
}

void KafkaConsumer::RebalanceCb(rd_kafka_t *rk,
        rd_kafka_resp_err_t err,
        rd_kafka_topic_partition_list_t *partitions,
        void *opaque)
{
    // fprintf(stderr, "%% Consumer group rebalanced: ");

    switch (err)
    {
        case RD_KAFKA_RESP_ERR__ASSIGN_PARTITIONS:
            // fprintf(stderr, "assigned:\n");
            // print_partition_list(stderr, partitions);
            rd_kafka_assign(rk, partitions);
            // wait_eof += partitions->cnt;
            break;

        case RD_KAFKA_RESP_ERR__REVOKE_PARTITIONS:
            // fprintf(stderr, "revoked:\n");
            // print_partition_list(stderr, partitions);
            rd_kafka_assign(rk, NULL);
            // wait_eof = 0;
            break;

        default:
            // fprintf(stderr, "failed: %s\n",
            //         rd_kafka_err2str(err));
            rd_kafka_assign(rk, NULL);
            break;
    }
}

