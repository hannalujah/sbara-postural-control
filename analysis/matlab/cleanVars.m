vars = who;
vars(strcmp(vars, 'data')) = [];
vars(strcmp(vars, 'Database')) = [];
vars(strcmp(vars, 'MeanComplete')) = [];
clear(vars{:});