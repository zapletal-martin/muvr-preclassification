%% Decides whether there is "no movement" in the input data
%

function [result, data] = nomovement(xs)
    %windowSize = 50;
    %b = (1/windowSize)*ones(1,windowSize);
    %a = 1;
    %relNums = filter(b, a, xs);
    
    m = mean(xs);
    m
    data = xs - m;
    result = max(data) - min(data) > 50;
end