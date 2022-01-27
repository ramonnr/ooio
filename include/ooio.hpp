#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <functional>
#include <fstream>
#include <iostream>
#include <dirent.h>

enum class Direction {
    IN,
    OUT
};

enum class Edge {
    NONE,
    RISING,
    FALLING
};

struct IOConfig {
    std::optional<std::string> name;
    std::optional<Direction> direction;
    std::optional<Edge> edge;
    std::optional<bool> active_low;
};

class IOConfigBuilder {
public:
    
    IOConfigBuilder() {};
    IOConfigBuilder& name(std::string_view ioname){
        _name = ioname;
        return *this;
    }

    IOConfigBuilder& direction(const Direction direction){
        _direction = direction;
        return *this;
    }
    IOConfigBuilder& edge(const Edge edge){
        _edge = edge;
        return *this;
    }
    IOConfigBuilder& active_low(const bool active_low) {
        _active_low = active_low;
        return *this;
    }

    IOConfig build(){
        return {
            _name,
            _direction,
            _edge,
            _active_low
        };
    }

private:
    std::optional<std::string> _name;
    std::optional<Direction> _direction;
    std::optional<Edge> _edge;
    std::optional<bool> _active_low;
};


class GPIO{
public:
    GPIO(const IOConfig&);
    virtual bool value() = 0;
    virtual void value(bool) = 0;
    virtual void on_change(std::function<void(bool)>&);
};


class FSGPIO{
public:
    FSGPIO(const IOConfig& config){
        export_io(config.name.value_or(""));
        set_dir(config.direction);
        set_edge(config.edge);
        set_active_low(config.active_low);
    }

    bool value(){
        std::ifstream ifs(io + "/value");
        std::string content;
        ifs >> content;
        ifs.close();
        return content == "1";
    }

    void value(bool v){
        std::ofstream ofs(io + "/value");
        ofs << v;
        ofs.flush();
        ofs.close();
    }

    bool is_ok() const {
        return is_good;
    }
    
private:
    void export_io(const std::string ionum){
        using namespace std::string_literals;
        std::string gpionum = "gpio"s + ionum;

        io = io_base + "/" + gpionum;

        if(auto dir = opendir(io.c_str()); dir){
            closedir(dir);
            return;
        }
        
        auto io_export = io_base + "/export";
        std::ofstream ofs(io_export);
        ofs << io;
        ofs.flush();
        if(!ofs.good()){
            std::clog
                << "unable to export: "
                << io
                << "\n";
            is_good = false;
        }
    }

    void set_dir(std::optional<Direction> dir){
        if(!dir){
            return;
        }
        const std::string dir_rep = [&]{
            if(*dir == Direction::IN){
                return "in";
            }
            return "out";
        }();
        std::ofstream ofs(io + "/direction");
        ofs << dir_rep;
        ofs.flush();
        if(!ofs.good()){
            std::clog
                << "unable to set direction for: "
                << io
                << "\n";
            is_good = false;
        }
    }

    void set_edge(std::optional<Edge> edge){
        if(!edge){
            return;
        }
        const std::string edge_rep = [&]{
            if(*edge == Edge::NONE){
                return "none";
            }
            else if(*edge == Edge::FALLING){
                return "falling";
            }
            return "rising";
        }();
        std::ofstream ofs(io + "/edge");
        ofs << edge_rep;
        ofs.flush();
        if(!ofs.good()){
            std::clog
                << "unable to set edge for: "
                << io
                << "\n";
            is_good = false;
        }
    }


    void set_active_low(std::optional<bool> active_low){
        if(!active_low){
            return;
        }
        std::ofstream ofs(io + "/edge");
        ofs << ((*active_low) ? "0" : "1");
        ofs.flush();
        if(!ofs.good()){
            std::clog
                << "unable to set active_low for: "
                << io
                << "\n";
            is_good = false;
        }
    }
    
    std::string io;
    const std::string io_base {"/sys/class/gpio/"};
    bool is_good = true;

};
