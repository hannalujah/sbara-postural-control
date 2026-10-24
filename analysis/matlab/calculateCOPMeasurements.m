%% reading all the files into a cell array

% Define your state and subject names
States = {
    '1. Pure Disturbance'
    '2. Cognitive Task Only'
    '3. Eyes Closed Only'
    '4. Eyes Closed + Cognitive Task'
    '5. Foam Only'
    '6. Foam + Cognitive Task'
    '7. Foam + Eyes Closed'
    '8. Foam + Eyes Closed + Cognitive Task'
    };

Subjects = {
    'Aaron'
    'Ali Mirzayi'
    'Ali Mojibi'
    'Dorsa'
    'Erfan Kashefi'
    'Kosar'
    'Mahsa'
    'Maryam'
    'Morteza'
    'Niloufar'
    'Reyhane'
    'Shamim'
    'Sina Fazeli'
    'Yasamin Az'
    'Yasamin Sh'
    };
minSampleNum = 347;
data = cell(length(Subjects), length(States));  % preallocate cell array

% Set your folder path (update this)
baseFolder = 'H:\Project codes and datas\testing codes\matlab code/';

for s = 1:length(States)
    for subj = 1:length(Subjects)
        stateName = States{s};
        subjectName = Subjects{subj};
        
        % Construct file name
        fileName = sprintf('%s, %s.txt', stateName, subjectName);
        filePath = fullfile(baseFolder, fileName);
        
        if ~isfile(filePath)
            fprintf('[MISSING] File not found: %s\n', fileName);
            continue;
        end
        
        fid = fopen(filePath, 'rt');
        if fid == -1
            warning('Could not open file: %s', fileName);
            continue;
        end
        
        % Skip the first header line
        fgetl(fid);
        
        % Read line by line
        rawData = [];
        while ~feof(fid)
            line = strtrim(fgetl(fid));
            if isempty(line) || all(isspace(line))
                continue;
            end
            % Split by commas
            parts = strsplit(line, ',');
            if numel(parts) == 3
                nums = str2double(parts);
                if all(~isnan(nums))
                    rawData(end+1, :) = nums;
                end
            end
        end
        fclose(fid);
        
        if isempty(rawData)
            fprintf('[EMPTY] No valid data in: %s\n', fileName);
        else
            data{subj, s} = rawData;
        end
    end
end

%% analyzing and refining each cell part

% so each cell block consists of all the data in a file;
% including the COPx, COPy and time
% we first need to turn each of these into a ~*9 matrix
% dividing them by when the diff of time is greater than 500
personalData = cell(1,3);
COPx = [];
COPy = [];
t = [];
lengthVex = []; % a vector to keep track of the lengths
fL = 0;
for s = 1:length(States)
    for subj = 1:length(Subjects)
        % extracting the copx, copy, and t for each subject and state
        COPx = data{subj, s}(:,1);
        COPy = data{subj, s}(:,2);
        t = data{subj, s}(:,3);
        
        % realigning each column to a 2D matrix
        COPx_initial = [];
        COPy_initial = [];
        t_initial = [];
        i = 1;
        rowIndex = 1;
        colIndex = 1;
        
        durations = diff(t);
        while (rowIndex < length(durations))
            dur = durations(1);
            if (rowIndex ~= 1)
                dur = t(rowIndex) - t(rowIndex - 1);
            end
            
            if (dur > 500)
                lengthVec(colIndex) = i;
                colIndex = colIndex + 1;
                rowIndex = rowIndex + 1;
                i = 1;
                continue;
            end
            
            COPx_initial(i, colIndex) = COPx(rowIndex);
            COPy_initial(i, colIndex) = COPy(rowIndex);
            t_initial(i, colIndex) = t(rowIndex);
            
            rowIndex = rowIndex + 1;
            i = i+1;
        end
        lengthVec(colIndex) = i;
        
        if fL == 0
            figure; hold on;
            for col = 1:size(COPx_initial, 2)
                colData = COPx_initial(:, col);
                i = 1;
                zeroIdx = find(colData == 0, i);% find first zero in this column
                if isempty(zeroIdx)
                    % No zeros found, plot full column
                    plot(colData);
                else
                    % Plot up to the element before the first zero
                    while zeroIdx < 200
                        i = i+1;
                        zeroIdx = find(colData == 0, i);
                    end
                    plot(colData(1:zeroIdx-1));
                end
            end
        end
        fL = 1;
        
        % determining which columns to keep
        colsToKeep = lengthVec > minSampleNum;
        COPx_initial = COPx_initial(:,colsToKeep);
        COPy_initial = COPy_initial(:,colsToKeep);
        t_initial = t_initial(:,colsToKeep);
        
        % last step of refining, delete all data before minSampleNum
        COPx = COPx_initial(1:minSampleNum, :);
        COPy = COPy_initial(1:minSampleNum, :);
        t = t_initial(1:minSampleNum, :);
        
        personalData{1} = COPx;
        personalData{2} = COPy;
        personalData{3} = t;
        
        data{subj,s} = personalData;
        
        % initializing for the next file
        COPx = 0;
        COPy = 0;
        t = 0;
        lengthVec = 0;
    end
end

%% clearing everything but the data cell
vars = who;
vars(strcmp(vars, 'data')) = [];
clear(vars{:});

%% Calculating all the measures for each data sample
% Total Values INIT
TotalSubNum = Subjects.TotalSubNum;
TotalStateNum = States.TotalStateNum;
TotalVarNum = CoPVars.TotalVarNum;
% Defining the DB
Database = zeros(TotalSubNum,TotalStateNum,TotalVarNum);

flag = 0;
flaG = 0;
figureFlag = 0;
figure;
% take out each column in each cell block in the data
for s = 1:TotalStateNum
    for subj = 1:TotalSubNum-1
        % going through each data cell block
        [~, colIndex] = size(data{subj,s}{1,1});
        internalDBPrime = zeros(TotalVarNum, colIndex);
        % going through each matrix individually
        for col = 1:colIndex
            COPx = data{subj,s}{1,1}(:,col);
            COPy = data{subj,s}{1,2}(:,col);
            t = data{subj,s}{1,3}(:,col);
            internalDB = zeros(1,TotalVarNum);
            calculateVars;
            internalDBCalc;
            internalDBPrime(:,col) = internalDB;
            if figureFlag == 0
                figure;
                figureFlag = 1;
            end
            if flag == 0
                plot(COPx);
                hold on;
            end
        end
        flag = 1;
        MeanVector = zeros(1,TotalVarNum);
        for row = 1:TotalVarNum
            MeanVector(row) = mean(internalDBPrime(row,:));
        end
        
        SubNum = subj;
        StateNum = s;
        addingVarsV2;
    end
end


meanVars;
MeanComplete = squeeze(Database(Subjects.Mean,:,:))';

%% clearing everything
vars = who;
vars(strcmp(vars, 'data')) = [];
vars(strcmp(vars, 'Database')) = [];
vars(strcmp(vars, 'MeanComplete')) = [];
clear(vars{:});

%% Export Database to EXCEL

%Data = load('databaseV4.mat');  % Load the file
A = Database;              % Replace 'varName' with your matrix name
[rows, cols, depth] = size(A);           % Get size of the 3D matrix
varLabels;

for k = 1:depth
    slice2D = A(1:15, :, k);                % Get the k-th 2D slice
    slice2D = round(slice2D, 4);
    sheetName = sprintf('%s', variableLabels{k});  % Create a sheet name
    writematrix(slice2D, 'DatabaseV5.xlsx', 'Sheet', sheetName);
end

%% Export MeanComplete
MeanComplete = round(MeanComplete,4);
writematrix(MeanComplete, 'Means.xlsx');
