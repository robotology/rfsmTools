/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio <nicogene@hotmail.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#include <StateGraphEditor.h>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <iostream>

using namespace rfsm;
using namespace std;


StateGraphEditor::StateGraphEditor():graph(NULL)
{

}

StateGraphEditor::StateGraphEditor(rfsm::StateGraph& graph):graph(&graph)
{

}

StateGraphEditor::~StateGraphEditor()
{

}

void StateGraphEditor::setGraph(rfsm::StateGraph &graph){
    this->graph=&graph;

}

void StateGraphEditor::addState(const string name, const string type)
{
    StateGraph::State state;
    state.type = type;
    state.name = name;
    if(find(graph->states.begin(),graph->states.end(),state) != graph->states.end())
        return;
    if(graph->states.size() == 0){
        StateGraph::State initial;
        initial.name="initial";
        initial.type="connector";
        graph->states.push_back(initial);
        if(state.type == "composit")
            addTransition(initial.name, state.name + ".initial");
        else if (state.type == "single")
            addTransition(initial.name, state.name);
    }
    graph->states.push_back(state);
    if(state.type == "composit")
        addState(name + ".initial", "connector");
}

void StateGraphEditor::removeState(const std::string name) {
    StateGraph::StateItr it;
    StateGraph::TransitionItr itFrom;
    StateGraph::TransitionItr itTo;
    for(it = graph->states.begin(); it<graph->states.end();) {
        StateGraph::State & st = *it;
        if((st.name == name) || (st.name.find(name+".") != string::npos))
        {
            //Remove all the transitions departing FROM the state
            do{
                itFrom=getTransition(st.name,true);
                if(itFrom != graph->transitions.end())
                    graph->transitions.erase(itFrom);
            }while(itFrom != graph->transitions.end());

            //Remove all the transitions TO the state

            do{
                itTo=getTransition(st.name,false);
                if(itTo != graph->transitions.end())
                    graph->transitions.erase(itTo);
            }while(itTo != graph->transitions.end());

            //Remove the state

            graph->states.erase(it);
        }
        else
            ++it;
    }
}

void StateGraphEditor::renameState(const std::string oldName, const std::string newName){
    if( oldName == newName)
        return;

    StateGraph::StateItr it;

    StateGraph::State st;
    st.name = newName;
    it = find(graph->states.begin(), graph->states.end(), st);
    assert(it == graph->states.end());

    st.name = oldName;
    it = find(graph->states.begin(), graph->states.end(), st);
    assert(it != graph->states.end());
    (*it).name = newName;

    if((*it).type=="composit")
    {
        for(StateGraph::StateItr i=graph->states.begin(); i<graph->states.end();i++)
        {
            StateGraph::State& s=*i;
            string oldNameChild=s.name;
            size_t itChild=s.name.find(oldName+".");
            if(itChild!= string::npos)
            {
                //s is a child
                string onlyChild= s.name.substr(itChild+oldName.size(),oldNameChild.size());
                s.name=newName+onlyChild;
                updateTransitions(oldNameChild,s.name);
            }

        }
    }
    updateTransitions(oldName,newName);


}

void StateGraphEditor::addTransition(const std::string source,
                               const std::string target,
                               std::vector<std::string> events){
    StateGraph::Transition transition;

    transition.source=source;
    transition.target=target;
    std::stringstream ss;
    transition.events=events;
    if(find(graph->transitions.begin(),graph->transitions.end(),transition) != graph->transitions.end())
        return;
    graph->transitions.push_back(transition);
}

void StateGraphEditor::removeTransition(const std::string source,
                      const std::string target,
                      std::vector<std::string> events) {
    StateGraph::TransitionItr it;
    for(it = graph->transitions.begin(); it<graph->transitions.end();) {
        StateGraph::Transition &tr = *it;
        if((tr.source == source) && (tr.target == target))
            if(events.size()) {
                 if(tr.events == events)
                     graph->transitions.erase(it);
            }
            else
                graph->transitions.erase(it);
        else
            ++it;
    }
}

void StateGraphEditor::addEvent(const std::string source,
              const std::string target,const std::string event){
    StateGraph::TransitionItr it;
    for(it = graph->transitions.begin(); it<graph->transitions.end();it++) {
        StateGraph::Transition &tr = *it;
        if((tr.source == source) && (tr.target == target)){
            assert(event!="");
            if(find(tr.events.begin(), tr.events.end(), event) != tr.events.end())
                return;
            tr.events.push_back(event);
        }
    }

}

void StateGraphEditor::clearEvents(const std::string source,
                 const std::string target)
{
    StateGraph::TransitionItr it;
    for(it = graph->transitions.begin(); it<graph->transitions.end();it++) {
        StateGraph::Transition &tr = *it;
        if((tr.source == source) && (tr.target == target))
            tr.events.clear();
    }

}

std::vector<std::string> StateGraphEditor::getEvents(const string source, const string target)
{
    StateGraph::TransitionItr it;
    for(it = graph->transitions.begin(); it<graph->transitions.end();it++) {
        StateGraph::Transition &tr = *it;
        if((tr.source == source) && (tr.target == target)){
            return tr.events;
        }
    }
}


void StateGraphEditor::getChilds(const std::string state, std::vector<std::string> &childs) {
    childs.clear();
    string name  = (state.size() == 0) ? "" : state+".";
    for(size_t i=0; i<graph->states.size(); i++) {
        if(name.size()) {
             string st = graph->states[i].name;
             if(st.find(name) != string::npos && getParent(st)==state)
                 childs.push_back(st);
        }
        else {
            string st = graph->states[i].name;
            if(st.find(".") == string::npos)
                childs.push_back(st);
        }
    }
}

StateGraph::State StateGraphEditor::getStateByName(const string stateName)
{
    StateGraph::State st;
    StateGraph::StateItr it;
    st.name = stateName;
    it = find(graph->states.begin(), graph->states.end(), st);
    assert(it != graph->states.end());
    return *it;
}

string StateGraphEditor::getParent(const string &stateName)
{
    size_t pos=stateName.find_last_of(".");
    return stateName.substr(0,pos);
}


StateGraph::TransitionItr StateGraphEditor::getTransition(const std::string stateName,
                                                    bool from,
                                                    std::vector<std::string> events) {
    StateGraph::TransitionItr it;
    for(it = graph->transitions.begin(); it<graph->transitions.end();) {
        StateGraph::Transition &tr = *it;
        bool decision;
        if(from)
            decision=tr.source == stateName;
        else
            decision=tr.target == stateName;
        if(decision)
            if(events.size()) {
                 if(tr.events == events)
                     return it;
            }
            else
                return it;
        else
            ++it;
    }
    return it;
}

void StateGraphEditor::updateTransitions(const std::string oldName, const std::string newName)
{
    StateGraph::TransitionItr itTrans;

    for(itTrans=graph->transitions.begin();itTrans<graph->transitions.end();itTrans++)
    {
        StateGraph::Transition& transition = *itTrans;
        if(transition.source==oldName)
            transition.source=newName;
        if(transition.target==oldName)
            transition.target=newName;
    }
}

